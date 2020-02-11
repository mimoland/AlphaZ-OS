/*
 * fat32文件系统
 */

#include <alphaz/kernel.h>
#include <alphaz/blkdev.h>
#include <alphaz/fat32.h>
#include <alphaz/string.h>
#include <alphaz/malloc.h>

#include <asm/bug.h>

disk_partition_table_t dpt;
fat32_boot_sector_t boot_sector;
fat32_fs_info_t     fs_info;

unsigned long first_data_sector;
unsigned long bytes_per_clus;
unsigned long first_fat1_sector, first_fat2_sector;

int fd;

static struct block_device_operations blkdev;

void dev_read(unsigned long nsect, unsigned long sector, void *buf)
{
    blkdev.transfer(BLK_READ, nsect, sector, buf);
}

unsigned int get_next_cluster(unsigned int entry)
{
    unsigned int buf[128];
    memset(buf, 0, 512);
    dev_read(first_fat1_sector + (entry >> 7), 1, buf);
    return buf[entry & 0x7f] & 0x0fffffff;
}

/* 匹配短目录项 */
static int match_director(char *name, unsigned int len, fat32_directory_t *dentry)
{
    int i, j = 0;
    char ch;
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
        default:
            ;
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
            default:
                ;
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
 * @len:  文件名或目录名的长度
 * @dentry: 长目录项的短目录项
 * 这里对长目录项的匹配并没有验证校验码，也并没有考虑长目录跨簇的问题
 */
static int match_long_directory(char *name, unsigned int len, fat32_directory_t *dentry)
{
    int i, j = 0;
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

fat32_directory_t * lookup(char *name, int len, fat32_directory_t *dentry, int flags)
{
    fat32_directory_t *next_dentry, *p;
    unsigned int cluster;
    unsigned long sector;
    unsigned char *buf;
    int i;

    buf = (unsigned char *)malloc(bytes_per_clus);
    cluster = (dentry->DIR_FstClusHI << 16 | dentry->DIR_FstClusLO) & 0x0fffffff;
    next_cluster:
    sector = first_data_sector + (cluster - 2) * boot_sector.BPB_SecPerClus;
    dev_read(sector, boot_sector.BPB_SecPerClus, buf);

    p = (fat32_directory_t *)buf;
    for (i = 0; i < bytes_per_clus; i += 32, p++) {
        if (p->DIR_Attr == ATTR_LONG_NAME)
            continue;
        if (p->DIR_Name[0] == 0xe5 || p->DIR_Name[0] == 0x00 || p->DIR_Name[0] == 0x05)
            continue;
        if (match_long_directory(name, len, p) || match_director(name, len, p)) {
            next_dentry = (fat32_directory_t *)malloc(sizeof(fat32_directory_t));
            *next_dentry = *p;
            free(buf);
            return next_dentry;
        }
    }
    cluster = get_next_cluster(cluster);
    if (cluster < 0x0ffffff7)
        goto next_cluster;
    free(buf);
    return NULL;
}

fat32_directory_t * path_walk(char *path, int flags)
{
    char *name, *p;
    int len;
    fat32_directory_t *parent, *next;

    p = path;
    while (*p == '/')
        p++;
    if (!*p)
        return NULL;

    parent = (fat32_directory_t *)malloc(sizeof(fat32_directory_t));
    parent->DIR_FstClusLO = boot_sector.BPB_RootClus & 0xffff;
    parent->DIR_FstClusHI = (boot_sector.BPB_RootClus >> 16) & 0x0fff;

    while (1) {
        name = p;
        len = 0;
        while (*p && *p != '/') {
            p++;
            len++;
        }

        next = lookup(name ,len, parent, flags);

        if (next == NULL) {
            free(parent);
            return NULL;
        }

        if (!*p) {
            break;
        }

        while (*p == '/')
            p++;
        if (!*p)
            break;

        free(parent);
        parent = next;
    }

    return next;
}

void fat32_init(void)
{
    blkdev = IDE_device_operation;
    char *p;
    int i;
    fat32_directory_t *dentry = NULL;
    dev_read(0, 1, &dpt);
    printk("dpt0 start LBA: %d\n", dpt.DPTE[0].start_LBA);
    dev_read(dpt.DPTE[0].start_LBA, 1, &boot_sector);
    printk("%s %d %d ", boot_sector.BS_OEMName, (int)boot_sector.BPB_TotSec32, (int)boot_sector.BPB_SecPerClus);
    for (p = (char *)boot_sector.BS_FilSysType, i = 0; i < 8; i++, p++)
        printk("%c", *p);
    printk("\n");
    dev_read(dpt.DPTE[0].start_LBA + boot_sector.BPB_FSInfo, 1, &fs_info);
    printk("0x%x 0x%x 0x%x 0x%x\n", fs_info.FSI_LeadSig, fs_info.FSI_StrucSig, fs_info.FSI_Free_Count, fs_info.FSI_Nxt_Free);

    first_data_sector = dpt.DPTE[0].start_LBA + boot_sector.BPB_RsvdSecCnt + boot_sector.BPB_FATSz32 * boot_sector.BPB_NumFATs;
    first_fat1_sector = dpt.DPTE[0].start_LBA + boot_sector.BPB_RsvdSecCnt;
    first_fat2_sector = first_fat1_sector + boot_sector.BPB_FATSz32;
    bytes_per_clus = boot_sector.BPB_SecPerClus * boot_sector.BPB_BytesPerSec;
    printk("%d %d %d %d\n", first_data_sector, first_fat1_sector, first_fat2_sector, bytes_per_clus);

    dentry = path_walk("/abc/b.txt", 0);
    if (dentry != NULL) {
        for (i = 0; i < 11; i++)
            printk("%c", dentry->DIR_Name[i]);
        printk("\n");
        printk("found: %d %d %d\n", dentry->DIR_FstClusHI, dentry->DIR_FstClusLO, dentry->DIR_FileSize);
        char *buf = (char *)malloc(bytes_per_clus);
        unsigned long cluster = (dentry->DIR_FstClusHI << 16 | dentry->DIR_FstClusLO) & 0x0fffffff;
        unsigned long sector = first_data_sector + (cluster - 2) * boot_sector.BPB_SecPerClus;
        dev_read(sector, 1, buf);
        for (i = 0; i < dentry->DIR_FileSize; i++)
            printk("%c", buf[i]);
    } else {
        printk("Don't found\n");
    }
}

