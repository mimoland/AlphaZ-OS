/*
 * IDE硬盘驱动程序，当前驱动还和x86体系相关，耦合度很高，我想先处理整体功能，然后在对整体结构
 * 进行调整。
 */

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
    outb(PORT_DISK0_DEVICE, 0xe0);      /* sector模式 */
}

void disk_exit(void)
{

}

/**
 * disk_handler - 硬盘中断处理函数
 * 硬盘会在写完一个扇区或者读完一个扇区后进行中断，所以一次硬盘设备请求可能会触发不止一次中断
 */
void disk_handler(void)
{
    disk_request_head.use->end_handler();
}


static void do_request(void)
{
    /*
     * do_request被调用时，可能已经没有了请求或者其他请求还没有执行完，若出现此类情况，
     * 则直接返回
     */
    if (!disk_request_head.count || disk_request_head.use)
        return;
    request_queue_t *r = list_first_entry(&disk_request_head.list, request_queue_t, list);
    disk_request_head.use = r;

    while (inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_BUSY) nop();

    switch (r->cmd)
    {
    case ATA_WRITE_CMD:
        outb(PORT_DISK0_DEVICE, 0xe0 | ((r->sector >> 24) & 0x0f));
        outb(PORT_DISK0_ERR_FEATURE, 0);
        outb(PORT_DISK0_SECTOR_CNT, r->nsect);
        outb(PORT_DISK0_SECTOR_LOW, r->sector & 0xff);
        outb(PORT_DISK0_SECTOR_MID, (r->sector >> 8) & 0xff);
        outb(PORT_DISK0_SECTOR_HIGH, (r->sector >> 16) & 0xff);

        while (!(inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_READY)) nop();
        outb(PORT_DISK0_STATUS_CMD, r->cmd);
        while (!(inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_REQ)) nop();
        outnw(PORT_DISK0_DATA, r->buf, SECTOR_SIZE / 2);
        break;

    case ATA_READ_CMD:
        outb(PORT_DISK0_DEVICE, 0xe0 | ((r->sector >> 24) & 0x0f));
        outb(PORT_DISK0_ERR_FEATURE, 0);
        outb(PORT_DISK0_SECTOR_CNT, r->nsect);
        outb(PORT_DISK0_SECTOR_LOW, r->sector & 0xff);
        outb(PORT_DISK0_SECTOR_MID, (r->sector >> 8) & 0xff);
        outb(PORT_DISK0_SECTOR_HIGH, (r->sector >> 16) & 0xff);
        while (!(inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_READY)) nop();
        outb(PORT_DISK0_STATUS_CMD, r->cmd);
        break;
    case GET_IDENTIFY_DISK_CMD:
        outb(PORT_DISK0_DEVICE, 0xe0 | ((r->sector >> 24) & 0x0f));
        outb(PORT_DISK0_ERR_FEATURE, 0);
        outb(PORT_DISK0_SECTOR_CNT, r->nsect);
        outb(PORT_DISK0_SECTOR_LOW, r->sector & 0xff);
        outb(PORT_DISK0_SECTOR_MID, (r->sector >> 8) & 0xff);
        outb(PORT_DISK0_SECTOR_HIGH, (r->sector >> 16) & 0xff);
        while (!(inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_READY)) nop();
        outb(PORT_DISK0_STATUS_CMD, r->cmd);
    default:
        printk("ATA CMD Error\n");
        break;
    }
}

static void end_request(void)
{
    list_del(&disk_request_head.use->list);
    disk_request_head.count--;
    free(disk_request_head.use);
    disk_request_head.use = NULL;
}

static void read_handler(void)
{
    request_queue_t *r = disk_request_head.use;
    if (inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_ERROR) {
        printk("read_handler: disk read error\n");
    } else {
        innw(PORT_DISK0_DATA, r->buf, SECTOR_SIZE / 2);
    }
    if (--r->nsect) {
        /* 若当前请求未读完，则继续等待下一个中断进行处理 */
        r->buf += SECTOR_SIZE;
        r->sector++;
        return;
    }
    end_request();
    do_request();
}

static void write_handler(void)
{
    request_queue_t *r = disk_request_head.use;
    if (inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_ERROR) {
        printk("read_handler: disk read error\n");
    }
    if (--r->nsect) {
        r->buf += SECTOR_SIZE;
        r->sector++;
        while (!(inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_REQ)) nop();
        outnw(PORT_DISK0_DATA, r->buf, SECTOR_SIZE / 2);
        return;
    }
    end_request();
    do_request();
}

static void identify_handler(void)
{
    request_queue_t *r = disk_request_head.use;
    if (inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_ERROR) {
        printk("read_handler: disk read error\n");
    } else {
        innw(PORT_DISK0_DATA, r->buf, SECTOR_SIZE / 2);
    }
    if (--r->nsect) {
        /* 若当前请求未读完，则继续等待下一个中断进行处理 */
        r->buf += SECTOR_SIZE;
        r->sector++;
        return;
    }
    end_request();
    do_request();
}

static request_queue_t * make_request(long cmd, unsigned long sector,
                                        unsigned long nsect, void *buf)
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
        r->end_handler = identify_handler;
        r->cmd = GET_IDENTIFY_DISK_CMD;
    default:
        r->end_handler = NULL;
        r->cmd = cmd;
        break;
    }

    r->sector = sector;
    r->nsect = nsect;
    r->buf = buf;

    return r;
}

static void put_request(request_queue_head_t *head, request_queue_t *r)
{
    list_add_tail(&head->list, &r->list);
    head->count++;
}

static long IDE_open(void)
{
    printk("open disk device: no operation\n");
    return 0;
}

static long IDE_release(void)
{
    printk("release disk device: no operation\n");
    return 0;
}

static long IDE_ioctl(long cmd, long arg)
{
    printk("ioctl disk device: no operation\n");
    return 0;
}

static long IDE_transfer(long cmd, unsigned long sector, unsigned long nsect, void *buf)
{
    request_queue_t *r = NULL;
    if (cmd == ATA_READ_CMD || cmd == ATA_WRITE_CMD) {
        r = make_request(cmd, sector, nsect, buf);
        put_request(&disk_request_head, r);
        do_request(); /* do_request不一定执行当前的请求，而是根据请求队列来执行 */
    } else
        return 0;
    return 1;
}

struct block_device_operations IDE_device_operation = {
    .open = IDE_open,
    .release = IDE_release,
    .ioctl = IDE_ioctl,
    .transfer = IDE_transfer,
};
