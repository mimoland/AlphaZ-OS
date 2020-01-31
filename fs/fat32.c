/*
 * fat32文件系统
 */

#include <alphaz/kernel.h>
#include <alphaz/blkdev.h>
#include <alphaz/fat32.h>

#include <asm/bug.h>

struct disk_partition_table dpt;
struct fat32_boot_sector boot_sector;
struct fat32_fs_info     fs_info;

static struct block_device_operations blkdev;

void dev_read(unsigned long nsect, unsigned long sector, void *buf)
{
    blkdev.transfer(BLK_READ, nsect, sector, buf);
    delay(3);
}

void fat32_init(void)
{
    blkdev = IDE_device_operation;

    char *p;
    int i;
    dev_read(0, 1, &dpt);
    printk("dpt0 start LBA: %d\n", dpt.DPTE[0].start_LBA);
    dev_read(dpt.DPTE[0].start_LBA, 1, &boot_sector);
    printk("%s %d %d ", boot_sector.BS_OEMName, (int)boot_sector.BPB_TotSec32, (int)boot_sector.BPB_SecPerClus);
    for (p = boot_sector.BS_FilSysType, i = 0; i < 8; i++, p++)
        printk("%c", *p);
    printk("\n");
    dev_read(dpt.DPTE[0].start_LBA + boot_sector.BPB_FSInfo, 1, &fs_info);
    printk("%x %x %x %x\n", fs_info.FSI_LeadSig, fs_info.FSI_StrucSig, fs_info.FSI_Free_Count, fs_info.FSI_Nxt_Free);
}

