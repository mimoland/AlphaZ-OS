/*
 * fat32文件系统
 */

#include <alphaz/blkdev.h>
#include <alphaz/bugs.h>
#include <alphaz/fat32.h>
#include <alphaz/fs.h>
#include <alphaz/kernel.h>
#include <alphaz/malloc.h>
#include <alphaz/string.h>

#include <asm/bug.h>

extern struct super_operations fat32_super_operations;
extern struct dentry_operations fat32_dentry_operations;
extern struct inode_operations fat32_inode_operations;
extern struct file_operations fat32_file_operations;

static struct block_device_operations blkdev;

static void dev_read(unsigned long sector, unsigned long nsect, void *buf)
{
    printk("dev_read: sector:%d nsect:%d\n", sector, nsect);
    blkdev.transfer(BLK_READ, sector, nsect, buf);
}

static unsigned int get_next_cluster(struct fat32_private_info *info,
                              unsigned int entry)
{
    unsigned int buf[128];
    memset(buf, 0, 512);
    dev_read(info->fat1_first_sector + (entry >> 7), 1, buf);
    return buf[entry & 0x7f] & 0x0fffffff;
}

/* 匹配短目录项 */
static int match_director(char *name, fat32_directory_t *dentry)
{
    int i, j = 0;
    char ch;
    int len = strlen(name);
    /* 匹配基础名 */
    for (i = 0; i < 8; i++) {
        switch (dentry->DIR_Name[i]) {
        case ' ':
            ch = dentry->DIR_Name[i] & 0xff;
            if (dentry->DIR_Attr & ATTR_DIRECTORY) {
                if (j < len && ch == name[j]) {
                    j++;
                    break;
                } else if (j == len)
                    continue;
                else
                    goto match_fail;
            } else {
                if (name[j] == '.')
                    continue;
                else
                    goto match_fail;
            }
        case 'A' ... 'Z':
        case 'a' ... 'z':
            if (dentry->DIR_NTRes & LOWERCASE_BASE)
                ch = (dentry->DIR_Name[i] + 32) & 0xff;
            else
                ch = dentry->DIR_Name[i] & 0xff;
            if (j < len && ch == name[j]) {
                j++;
                break;
            } else
                goto match_fail;
        case '0' ... '9':
            ch = dentry->DIR_Name[i];
            if (j < len && ch == name[j]) {
                j++;
                break;
            } else
                goto match_fail;
        default:;
        }
    }

    /* 匹配扩展名 */
    if (!(dentry->DIR_Attr & ATTR_DIRECTORY)) {
        j++; // 跳过'.'
        for (i = 8; i < 11; i++) {
            switch (dentry->DIR_Name[i]) {
            case ' ':
                ch = dentry->DIR_Name[i];
                if (j < len && ch == name[j]) {
                    j++;
                    break;
                } else
                    goto match_fail;
            case 'A' ... 'Z':
            case 'a' ... 'z':
                if (dentry->DIR_NTRes && LOWERCASE_EXT)
                    ch = (dentry->DIR_Name[i] + 32) & 0xff;
                else
                    ch = dentry->DIR_Name[i] & 0xff;
                if (j < len && ch == name[j]) {
                    j++;
                    break;
                } else
                    goto match_fail;
            case '0' ... '9':
                ch = dentry->DIR_Name[i] & 0xff;
                if (j < len && ch == name[j]) {
                    j++;
                    break;
                } else
                    goto match_fail;
            default:;
            }
        }
    }
    return 1;
match_fail:
    return 0;
}

/**
 * 匹配长目录项
 * @name: 要匹配的文件名(目录名)
 * @dentry: 长目录项的短目录项
 * 这里对长目录项的匹配并没有验证校验码，也并没有考虑长目录跨簇的问题
 */
static int match_long_directory(char *name, fat32_directory_t *dentry)
{
    int i, j = 0;
    int len = strlen(name);
    fat32_long_directory_t *ldentry = (fat32_long_directory_t *)dentry - 1;
    while (ldentry->LDIR_Attr == ATTR_LONG_NAME && ldentry->LDIR_Ord != 0xe5) {
        for (i = 0; i < 5; i++) {
            if (j < len && ldentry->LDIR_Name1[i] != (unsigned short)name[j++])
                goto match_fail;
        }
        for (i = 0; i < 6; i++) {
            if (j < len && ldentry->LDIR_Name2[i] != (unsigned short)name[j++])
                goto match_fail;
        }
        for (i = 0; i < 2; i++) {
            if (j < len && ldentry->LDIR_Name3[i] != (unsigned short)name[j++])
                goto match_fail;
        }
        if (j >= len)
            goto match_success;
        ldentry--;
    }
match_fail:
    return 0;
match_success:
    return 1;
}

/*
 * 根据FAT32文件系统的目录项entry，为de创建de->d_inode
 */
static struct inode *make_inode(fat32_directory_t *entry, struct dentry *de)
{
	struct inode *node;

	node = (struct inode *)kmalloc(sizeof(struct inode), 0);
	assert(node != NULL);
	list_add(&node->i_sb_list, &de->d_sb->s_inodes);
	node->i_ino = entry->DIR_FstClusHI << 16 | entry->DIR_FstClusLO;
	atomic_set(1, &node->i_count);
	node->i_op = &fat32_inode_operations;
    node->i_fop = &fat32_file_operations;
	spin_init(&node->i_lock);
	node->i_size = entry->DIR_FileSize;
	node->i_sb = de->d_sb;
	node->i_state = 0;
	node->i_private = NULL;
	return node;
}

static struct inode * alloc_inode(struct super_block *sb) { return NULL; }
static void destory_inode(struct inode *inode) {}
static void write_inode(struct inode *inode) {}
static void delete_inode(struct inode *inode) {}
static void write_super(struct super_block *sb) {}
static void put_super(struct super_block *sb) {}

struct super_operations fat32_super_operations = {
    .alloc_inode = alloc_inode,
    .destory_inode = destory_inode,
    .write_inode = write_inode,
    .delete_inode = delete_inode,
    .write_super = write_super,
    .put_super = put_super,
};

static int create(struct inode *inode, struct dentry *dentry, int mode) { return -1; }

/* 从节点dir中根据目录项de中的文件名找出对应的文件，并为de创建de->d_inode。若找不到，
 * 则返回NULL
 */
static struct dentry *lookup(struct inode *dir, struct dentry *de)
{
    fat32_directory_t *next_dentry, *p;
    struct fat32_private_info *private;
    unsigned int cluster;
    unsigned long sector;
    unsigned char *buf;
    int i;

    private = dir->i_sb->s_fs_info;
    buf = (unsigned char *)kmalloc(private->bytes_per_clus, 0);
    cluster = dir->i_ino; /* ino为文件的第一个簇的簇号 */
next_cluster:
    sector = private->data_first_sector + (cluster - 2) * private->sector_per_clus;
    dev_read(sector, private->sector_per_clus, buf);

    p = (fat32_directory_t *)buf;
    for (i = 0; i < private->bytes_per_clus; i += 32, p++) {
        if (p->DIR_Attr == ATTR_LONG_NAME)
            continue;
        if (p->DIR_Name[0] == 0xe5 || p->DIR_Name[0] == 0x00 ||
            p->DIR_Name[0] == 0x05)
            continue;
        if (match_long_directory(de->d_name, p) ||
            match_director(de->d_name, p)) {
            next_dentry =
                (fat32_directory_t *)kmalloc(sizeof(fat32_directory_t), 0);
            *next_dentry = *p;
            goto founded;
        }
    }
    cluster = get_next_cluster(private, cluster);
    if (cluster < 0x0ffffff7)
        goto next_cluster;
    kfree(buf);
    return NULL;
founded:
    kfree(buf);
    de->d_inode = make_inode(next_dentry, de);
    return de;
}

static int mkdir(struct inode *dir, struct dentry *dentry, int mode) { return -1; }
static int rmdir(struct inode *dir, struct dentry *dentry) { return -1; }
static int rename(struct inode *old_dir, struct dentry *d1,
                  struct inode *new_dir, struct dentry *d2)
{
    return -1;
}

static int getattr(struct dentry *dentry, unsigned long mode) { return -1; }
static int setattr(struct dentry *dentry, unsigned long mode) { return -1; }

struct inode_operations fat32_inode_operations = {
    .create = create,
	.lookup = lookup,
    .mkdir = mkdir,
    .rmdir = rmdir,
    .rename = rename,
    .getattr = getattr,
    .setattr = setattr,
};

static int d_compare(struct dentry *dentry, char *name1, char *name2) { return -1; }
static int d_release(struct dentry *dentry) { return -1; }
static int d_iput(struct dentry *dentry, struct inode *inode) { return -1; }

struct dentry_operations fat32_dentry_operations = {
    .d_compare = d_compare,
    .d_release = d_release,
    .d_iput = d_iput,
};

static loff_t lseek(struct file *filp, loff_t offset, int whence)
{
    assert(filp != NULL);
    switch (whence) {
    case SEEK_SET:
        filp->f_pos = offset;
        break;
    case SEEK_CUR:
        filp->f_pos += offset;
        break;
    case SEEK_END:
        filp->f_pos = filp->f_dentry->d_inode->i_size + offset;
        break;
    default:
        break;
    }
    return filp->f_pos;
}

static ssize_t read(struct file *filp, char *buf, size_t size, loff_t pos)
{
    struct fat32_private_info *private;
    unsigned long index, offset, clus, sector, size1, count;
    char *buffer;
    int i;

    assert(filp != NULL);
    if (pos >= filp->f_dentry->d_inode->i_size)  /* 大于文件的大小 */
        return -1;

    private = filp->f_dentry->d_sb->s_fs_info;
    index = pos / private->bytes_per_clus;
    offset = pos % private->bytes_per_clus;
    clus = filp->f_dentry->d_inode->i_ino;      /* 第一个簇 */
    count = 0;
    buffer = (char *)kmalloc(sizeof(private->bytes_per_clus), 0);
    assert(buffer != NULL);

    for (i = 0; i < index; i++)
        clus = get_next_cluster(private, clus);

    if (pos + size >= filp->f_dentry->d_inode->i_size)
        size = filp->f_dentry->d_inode->i_size - pos;

    do {
        sector = private->data_first_sector + (clus - 2) * private->sector_per_clus;
        dev_read(sector, private->sector_per_clus, buffer);

        size1 = (private->bytes_per_clus - offset) > size ? size :
                    private->bytes_per_clus - offset;
        memcpy(buf, buffer + offset, size1);

        buf = buf + size1;
        offset = 0;
        size -= size1;
        count += size1;
        clus = get_next_cluster(private, clus);
    } while(size > 0);
    return count;
}

static ssize_t write(struct file *filp, const char *buf, size_t size, loff_t pos) { return -1; }
static int open(struct inode *inode, struct file *filp) { return -1; }
static int release(struct inode *inode, struct file *filp) { return -1; }

struct file_operations fat32_file_operations = {
    .lseek = lseek,
    .read = read,
    .write = write,
    .open = open,
    .release = release,
};

static struct super_block *fat32_get_sb(struct file_system_type *fs, void *info)
{
	disk_partition_table_t *dpt;
	disk_partition_table_entry_t *dpte;
	fat32_boot_sector_t *boot_sector;
	fat32_fs_info_t *fs_info;
	struct fat32_private_info *private;
	struct super_block *sb;
	struct dentry *s_root;
	struct inode *d_inode;

	dpt = (disk_partition_table_t *)kmalloc(512, 0);
	boot_sector = (fat32_boot_sector_t *)kmalloc(sizeof(fat32_boot_sector_t), 0);
	fs_info = (fat32_fs_info_t *)kmalloc(sizeof(fat32_fs_info_t), 0);
    private = (struct fat32_private_info *)kmalloc(sizeof(struct fat32_private_info), 0);
	sb = (struct super_block *)kmalloc(sizeof(struct super_block), 0);
	s_root = (struct dentry *)kmalloc(sizeof(struct dentry), 0);
	d_inode = (struct inode *)kmalloc(sizeof(struct inode), 0);

	dev_read(0, 1, dpt);
	dpte = &dpt->DPTE[0];

	dev_read(dpte->start_LBA, 1, boot_sector);
	dev_read(dpte->start_LBA + boot_sector->BPB_FSInfo, 1, fs_info);
    private->start_sector = dpte->start_LBA;
    private->sector_count = dpte->sectors_limit;
    private->sector_per_clus = boot_sector->BPB_SecPerClus;
    private->bytes_per_clus = boot_sector->BPB_SecPerClus * boot_sector->BPB_BytesPerSec;
    private->bytes_per_sector = boot_sector->BPB_BytesPerSec;
    private->data_first_sector = dpte->start_LBA + boot_sector->BPB_RsvdSecCnt +
                            boot_sector->BPB_FATSz32 * boot_sector->BPB_NumFATs;
    private->fat1_first_sector = dpte->start_LBA + boot_sector->BPB_RsvdSecCnt;
    private->sector_per_fat = boot_sector->BPB_FATSz32;
    private->num_of_fats = boot_sector->BPB_NumFATs;
    private->fsinfo_sector_infat = boot_sector->BPB_FSInfo;
    private->bootsector_bk_infat = boot_sector->BPB_BkBootSec;
    private->first_cluster = boot_sector->BPB_RootClus;

	sb->s_blocksize = private->bytes_per_sector;
	sb->s_type = fs;
	sb->s_op = &fat32_super_operations;
	sb->s_root = s_root;
	sb->s_fs_info = private;
	list_head_init(&sb->s_inodes);
	list_head_init(&sb->s_dirty);

	atomic_set(1, &s_root->d_count);
	s_root->d_flags = 0;
	spin_init(&s_root->d_lock);
	s_root->d_inode = d_inode;
	s_root->d_parent = NULL;
	strcpy(s_root->d_name, "/");
	list_head_init(&s_root->d_child);
	list_head_init(&s_root->d_subdirs);
	s_root->d_op = &fat32_dentry_operations;
	s_root->d_sb = sb;
	s_root->d_private = NULL;

	list_add(&d_inode->i_sb_list, &sb->s_inodes);
	list_head_init(&d_inode->i_denty);
	d_inode->i_ino =
		boot_sector->BPB_RootClus; // 以根目录的第一个簇的簇号作为ino
	atomic_set(1, &d_inode->i_count);
	d_inode->i_op = &fat32_inode_operations;
    d_inode->i_fop = &fat32_file_operations;
	spin_init(&d_inode->i_lock);
	d_inode->i_size = sizeof(struct super_block); // 节点的大小为超级块大小
	d_inode->i_sb = sb;
	d_inode->i_state = 0;
	d_inode->i_private = NULL;

	kfree(dpt);
	kfree(dpte);
	kfree(boot_sector);
	kfree(fs_info);

	return sb;
}

static void fat32_put_sb(struct super_block *sb)
{
	kfree(sb->s_fs_info);
	kfree(sb->s_root);
	kfree(sb);
}

struct file_system_type fat32_fs_type = {
	.name = "FAT32",
	.fs_flags = 0,
	.get_sb = fat32_get_sb,
	.put_sb = fat32_put_sb,
};

void fat32_init(void)
{
	blkdev = IDE_device_operation;

	if (register_filesystem(&fat32_fs_type) != 0) {
		panic("file system register error\n");
		return;
	}
}
