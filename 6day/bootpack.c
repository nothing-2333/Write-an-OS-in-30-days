#include <stdio.h>

#include "bootpack.h"
#include "desctbl.h"
#include "graphic.h"
#include "int.h"
#include "io.h"

int main(void)
{
    BootInfo *binfo = (BootInfo*)ADR_BOOTINFO;
    char mcursor[256];

    // 初始化gdt、idt
    init_gdt_idt();

    // GDT/IDT完成初始化，开放CPU中断
    init_pic();
 
    // 允许中断请求被处理
    io_sti();

    // 设定调色板
    init_palette();

    init_screen(binfo->vram, binfo->scrn_x, binfo->scrn_y);

    // 鼠标初始位置
    int m_x = (binfo->scrn_x - 16) / 2;
    int m_y = (binfo->scrn_y - 28 - 16) / 2;
    init_mouse_cursor8(mcursor, COL8_00477D);
    put_block8_8(binfo->vram, binfo->scrn_x, 16, 16, m_x, m_y, mcursor, 16);

    // 显示字
    char s[40];
    sprintf(s, "nothing_%d", 2333);
    put_fonts8_asc(binfo->vram, binfo->scrn_x, 10, 10, COL8_FFFFFF, s);

    // 开放PIC1以及键盘中断
    io_out8(PIC0_IMR, 0xf9);
    // 开放鼠标中断
    io_out8(PIC1_IMR, 0xef);

    for (;;) io_hlt();
    return 0;
}

