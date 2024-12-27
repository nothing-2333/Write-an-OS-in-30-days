#include "bootpack.h"
#include "graphic.h"
#include "int.h"
#include "io.h"

void init_pic()
{
    // 禁止所有中断
    io_out8(PIC0_IMR, 0xff);
    io_out8(PIC1_IMR, 0xff);

    // 边缘触发模式
    io_out8(PIC0_ICW1, 0x11);
    // IRQ0-7由INT20-27接收
    io_out8(PIC0_ICW2, 0x20);
    // PIC1由IRQ2连接
    io_out8(PIC0_ICW3, 1 << 2);
    // 无缓冲区模式
    io_out8(PIC0_ICW4, 0x1);

    // 边缘触发模式
    io_out8(PIC1_ICW1, 0x11);
    // IRQ8-15由INT28-2f接收
    io_out8(PIC1_ICW2, 0x28);
    // PIC1由IRQ2连接
    io_out8(PIC1_ICW3, 2);
    // 无缓冲区模式
    io_out8(PIC1_ICW4, 0x1);

    // PIC1以外中断全部禁止
    io_out8(PIC0_IMR, 0xfb);
    // 禁止全部中断
    io_out8(PIC1_IMR, 0xff);
}

void int_handler21(int* esp)
{
    BootInfo* binfo = (BootInfo*)ADR_BOOTINFO;

    box_fill8(binfo->vram, binfo->scrn_x, COL8_000000, 0, 0, 32 * 8 - 1, 15);
    put_fonts8_asc(binfo->vram, binfo->scrn_x, 0, 0, COL8_FFFFFF, "keyboard");

    for (;;) io_hlt();
}

void int_handler2c(int* esp)
{
    BootInfo* binfo = (BootInfo*)ADR_BOOTINFO;

    box_fill8(binfo->vram, binfo->scrn_x, COL8_000000, 0, 0, 32 * 8 - 1, 15);
    put_fonts8_asc(binfo->vram, binfo->scrn_x, 0, 0, COL8_FFFFFF, "mouse");

    for (;;) io_hlt();
}

void int_handler27(int* esp)
{
    io_out8(PIC0_OCW2, 0x67);
}