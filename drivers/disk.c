#include <alphaz/kernel.h>
#include <alphaz/list.h>
#include <alphaz/malloc.h>

#include <asm/disk.h>
#include <asm/bug.h>
#include <asm/irq.h>
#include <asm/io.h>

static request_queue_head_t disk_request_head;

/**
 * disk_init - ATA硬盘初始化
 * 为了简单起见，这里使用基于IDT的I/O端口编程模式对ATA硬盘进行访问，而不使用SATA控制器和PCI
 * 总线控制器来访问
 */
void disk_init(void)
{
    INIT_REQUEST_QUEST_HEAD(&disk_request_head);
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

void disk_exit(void)
{

}

/**
 * disk_handler - 硬盘中断处理函数
 */
void disk_handler(void)
{
    disk_request_head.use->end_handler();
}


static void exec_request(void)
{
    request_queue_t *r = list_first_entry(&disk_request_head.list, request_queue_t, list);
    list_del(&r->list);
    disk_request_head.use = r;
    disk_request_head.count--;

    while (inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_BUSY) nop();

    switch (r->cmd)
    {
    case ATA_WRITE_CMD:
        printk("write\n");
        outb(PORT_DISK0_DEVICE, 0xe0 | ((r->LBA >> 24) & 0x0f));
        outb(PORT_DISK0_ERR_FEATURE, 0);
        outb(PORT_DISK0_SECTOR_CNT, r->count & 0xff);
        outb(PORT_DISK0_SECTOR_LOW, r->LBA & 0xff);
        outb(PORT_DISK0_SECTOR_MID, (r->LBA >> 8) & 0xff);
        outb(PORT_DISK0_SECTOR_HIGH, (r->LBA >> 16) & 0xff);

        while (!(inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_READY)) nop();
        outb(PORT_DISK0_STATUS_CMD, r->cmd);
        while (!(inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_REQ)) nop();
        outnw(PORT_DISK0_DATA, r->buf, 256);
        break;

    case ATA_READ_CMD:
        printk("read\n");
        outb(PORT_DISK0_DEVICE, 0xe0 | ((r->LBA >> 24) & 0x0f));
        outb(PORT_DISK0_ERR_FEATURE, 0);
        outb(PORT_DISK0_SECTOR_CNT, r->count & 0xff);
        outb(PORT_DISK0_SECTOR_LOW, r->LBA & 0xff);
        outb(PORT_DISK0_SECTOR_MID, (r->LBA >> 8) & 0xff);
        outb(PORT_DISK0_SECTOR_HIGH, (r->LBA >> 16) & 0xff);
        while (!(inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_READY)) nop();
        outb(PORT_DISK0_STATUS_CMD, r->cmd);
        break;
    case GET_IDENTIFY_DISK_CMD:
        outb(PORT_DISK0_DEVICE, 0xe0 | ((r->LBA >> 24) & 0x0f));
        outb(PORT_DISK0_ERR_FEATURE, 0);
        outb(PORT_DISK0_SECTOR_CNT, r->count & 0xff);
        outb(PORT_DISK0_SECTOR_LOW, r->LBA & 0xff);
        outb(PORT_DISK0_SECTOR_MID, (r->LBA >> 8) & 0xff);
        outb(PORT_DISK0_SECTOR_HIGH, (r->LBA >> 16) & 0xff);
        while (!(inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_READY)) nop();
        outb(PORT_DISK0_STATUS_CMD, r->cmd);
    default:
        printk("ATA CMD Error\n");
        break;
    }
}

static void end_request(void)
{
    free(disk_request_head.use);
    disk_request_head.use = NULL;
    if (disk_request_head.count)
        exec_request();
}

static void read_handler()
{
    request_queue_t *r = disk_request_head.use;
    if (inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_ERROR) {
        printk("read_handler: disk read error\n");
    } else {
        innw(PORT_DISK0_DATA, r->buf, 256);
    }
    end_request();
}

static void write_handler()
{
    if (inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_ERROR) {
        printk("read_handler: disk read error\n");
    }
    end_request();
}


static struct disk_identify_info_struct info;

static void disk_identify(void)
{
    int i;

    innw(PORT_DISK0_DATA, &info, 256);
    printk("disk info: \n");

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

static request_queue_t * make_request(long cmd, unsigned long blocks,
                                        long count, void *buf)
{
    request_queue_t *r = (request_queue_t *)malloc(sizeof(request_queue_t));

    switch (cmd)
    {
    case ATA_READ_CMD:
        r->end_handler = read_handler;
        r->cmd = ATA_READ_CMD;
        break;
    case ATA_WRITE_CMD:
        r->end_handler = write_handler;
        r->cmd = ATA_WRITE_CMD;
        break;
    case GET_IDENTIFY_DISK_CMD:
        r->end_handler = disk_identify;
        r->cmd = GET_IDENTIFY_DISK_CMD;
    default:
        r->end_handler = NULL;
        r->cmd = cmd;
        break;
    }

    r->LBA = blocks;
    r->count = count;
    r->buf = buf;

    return r;
}

static void put_request(request_queue_head_t *head, request_queue_t *r)
{
    list_add_tail(&head->list, &r->list);
    head->count++;
    if (disk_request_head.use == NULL) {
        exec_request();
    }
}

static long IDE_open(void)
{
    return 0;
}

static long IDE_close(void)
{
    return 0;
}


static long IDE_ioctl(long cmd, long arg)
{
    return 0;
}

static long IDE_transfer(long cmd, unsigned long blocks, long count, void *buf)
{
    request_queue_t *r = NULL;
    if (cmd == ATA_READ_CMD || cmd == ATA_WRITE_CMD) {
        r = make_request(cmd, blocks, count, buf);
        put_request(&disk_request_head, r);
    } else {
        return 0;
    }
    return 1;
}

struct block_device_operation IDE_device_operation =
{
    .open = IDE_open,
    .close = IDE_close,
    .ioctl = IDE_ioctl,
    .transfer = IDE_transfer,
};
