#include <alphaz/string.h>
#include <alphaz/type.h>
#include <alphaz/malloc.h>
#include <alphaz/kernel.h>
#include <alphaz/fs.h>
#include <alphaz/bugs.h>
#include <asm/irq.h>

struct super_block *root_sb = NULL;
struct dentry *root_dentry = NULL;

/**
 * register_filesystem - 注册文件系统
 * @fs: 要注册的文件系统指针
 */
int register_filesystem(struct file_system_type *fs)
{
    if (!fs)
        return -1;
    root_sb = fs->get_sb(fs, NULL);
    if (!root_sb)
        return -1;
    return 0;
}

/**
 * mount_fs - 挂载文件系统
 * @name: 要挂载的文件系统的名称
 * @sb:   要挂载的文件系统的超级块指针
 */
int mount_fs(const char *name, struct super_block *sb)
{
    return 0;
}

/* 为parent创建一个子dentry并进行必要的初始化 */
static struct dentry * make_dentry(struct dentry *parent, char *name, size_t len)
{
    struct dentry *entry;

    entry = (struct dentry *)kmalloc(sizeof(struct dentry), 0);
    assert(entry != NULL);
    atomic_set(1, &entry->d_count);
    entry->d_flags= 0;
    spin_init(&entry->d_lock);
    entry->d_inode = NULL;
    entry->d_parent = parent;
    memcpy(entry->d_name, name, len);
    entry->d_name[len] = 0;
    list_add_tail(&entry->d_child, &parent->d_subdirs);
    list_head_init(&entry->d_subdirs);
    entry->d_op = parent->d_op;
    entry->d_sb = parent->d_sb;
    entry->d_private = parent->d_private;
    return entry;
}

struct dentry * path_walk(char *path, int flags)
{
    char *name, *p;
    int len;
    struct dentry *parent = root_sb->s_root;
    struct dentry *child;

    p = path;
    while (*p == '/')
        p++;
    if (!*p)
        return NULL;

    while (1) {
        name = p;
        len = 0;
        while (*p && *p != '/') {
            p++;
            len++;
        }

        printk("%p\n", parent);
        child = make_dentry(parent, name, len);
        printk("child: %p\n", child);
        if (parent->d_inode->i_op->lookup(parent->d_inode, child) == NULL) {
            panic("cound't found file");
            free(child);
            return NULL;
        }

        if (!*p) {
            break;
        }

        while (*p == '/')
            p++;
        if (!*p)
            break;

        parent = child;
        child = NULL;
    }
    return child;
}
