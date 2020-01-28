#include <alphaz/kernel.h>

#include <asm/disk.h>
#include <asm/irq.h>
#include <asm/io.h>

/**
 * disk_init - ATA硬盘初始化
 * 为了简单起见，这里使用基于IDT的I/O端口编程模式对ATA硬盘进行访问，而不使用SATA控制器和PCI
 * 总线控制器来访问
 */
void disk_init(void)
{
    printk("disk_init");
    enable_irq(0x2);        /* 主片级联 */
    enable_irq(0xe);
    outb(PORT_DISK0_ALT_STA_CTL, 0);
    outb(PORT_DISK0_ERR_FEATURE , 0);
    outb(PORT_DISK0_SECTOR_CNT, 0);
    outb(PORT_DISK0_SECTOR_LOW, 0);
    outb(PORT_DISK0_SECTOR_MID, 0);
    outb(PORT_DISK0_SECTOR_HIGH, 0);
    outb(PORT_DISK0_DEVICE, 0xe0);      /* LBA模式 */
}


void disk_handler(void)
{
    int i;
    struct disk_identify_info_struct info;

    innw(PORT_DISK0_DATA, &info, 256);
    printk("disk info: %c%c\n", '1', '2');

    printk("Serial Number: ");      /* 序列号 */
    for (i = 0; i < 10; ++i) {
        printk("%c%c", (info.Serial_Number[i] >> 8) & 0xff, info.Serial_Number[i] & 0xff);
    }
    printk("\n");

    printk("Firm version: ");     /* 固件版本 */
    for (i = 0; i < 4; ++i) {
        printk("%c%c", (info.Firmware_Version[i] >> 8) & 0xff, info.Firmware_Version[i] & 0xff);
    }
    printk("\n");

    printk("Model Number: ");     /* 型号 */
    for (i = 0; i < 20; ++i) {
        printk("%c%c", (info.Model_Number[i] >> 8) & 0xff, info.Model_Number[i] & 0xff);
    }
    printk("\n");

    printk("LBA supported: %s\n", (info.Capabilities0 & 0x0200)  ? "Yes" : "No");
    printk("LBA48 supported: %s\n", (info.Capabilities0 & 0x0400) ? "Yes": "No");
    printk("Total Sector(for 28 bit): %d\n", (int)info.Addressable_Logical_Sectors_for_28[0]
            | ((int)info.Addressable_Logical_Sectors_for_28[1] << 16));
}
