#include <stdio.h>

#include "bootpack.h"
#include "desctbl.h"
#include "fifo.h"
#include "graphic.h"
#include "int.h"
#include "io.h"
#include "keyboard.h"
#include "mem.h"
#include "mouse.h"

int main(void)
{    
    // 初始化gdt、idt
    init_gdt_idt();

    // GDT/IDT完成初始化，开放CPU中断
    init_pic();
 
    // 允许中断请求被处理
    io_sti();

    // 初始化缓冲区
    fifo8_init(&keyfifo, KEY_FIFO_BUF_SIZE, keybuf);
    fifo8_init(&mousefifo, MOUSE_FIFO_BUF_SIZE, mousebuf);

    // 开放PIC1以及键盘中断
    io_out8(PIC0_IMR, 0xf9);
    // 开放鼠标中断
    io_out8(PIC1_IMR, 0xef);

    // 初始化键盘
    init_keyboard();
    
    // 设定调色板
    init_palette();

    // 写显存初始化屏幕
    char temp[40];
    BootInfo *binfo = (BootInfo*)ADR_BOOTINFO;
    init_screen(binfo->vram, binfo->scrn_x, binfo->scrn_y);

    // 鼠标初始位置
    int m_x = (binfo->scrn_x - 16) / 2;
    int m_y = (binfo->scrn_y - 28 - 16) / 2;
    char mcursor[256];
    init_mouse_cursor8(mcursor, COL8_00477D);
    put_block8_8(binfo->vram, binfo->scrn_x, 16, 16, m_x, m_y, mcursor, 16);
    sprintf(temp, "(%d, %d)", m_x, m_y);
    put_fonts8_asc(binfo->vram, binfo->scrn_x, 0, 0, COL8_FFFFFF, temp);

    // 启动鼠标中断
    MouseDec mdec;
    enable_mouse(&mdec);

    // 初始化内存
    unsigned int memtotal = memtest(0x400000, 0xbfffffff);
    MemMan* memman = (MemMan*)MEMMAN_ADDR;
    memman_init(memman);
    memman_free(memman, 0x1000, 0x9e000);
    memman_free(memman, 0x400000, memtotal - 0x400000);

    sprintf(temp, "memory %dMB, free: %dKB", memtotal / (1024 * 1024), memman_total(memman) / 1024);
    put_fonts8_asc(binfo->vram, binfo->scrn_x, 0, 32, COL8_FFFFFF, temp);
    
    while (1)
    {
        io_cli();
        if (fifo8_status(&keyfifo) == 0 && fifo8_status(&mousefifo) == 0)
        {
            io_stihlt();
        }
        else if (fifo8_status(&keyfifo))
        {
            unsigned char key = (unsigned char)fifo8_get(&keyfifo);
            io_sti();
            sprintf(temp, "%02x", key);
            box_fill8(binfo->vram, binfo->scrn_x, COL8_00477D, 0, 16, 15, 31);
            put_fonts8_asc(binfo->vram, binfo->scrn_x, 0, 16, COL8_FFFFFF, temp);
        }
        else if (fifo8_status(&mousefifo))
        {
            unsigned char key = (unsigned char)fifo8_get(&mousefifo);
            io_sti();

            if (mouse_decode(&mdec, key))
            {
                // 擦除上次绘制
                box_fill8(binfo->vram, binfo->scrn_x, COL8_00477D, 32, 16, 32 + 15 * 8 - 1, 31);
                box_fill8(binfo->vram, binfo->scrn_x, COL8_00477D, m_x, m_y, m_x + 15, m_y + 15);
                box_fill8(binfo->vram, binfo->scrn_x, COL8_00477D, 0, 0, 79, 15);

                // 更新
                sprintf(temp, "[lcr %4d %4d]", mdec.x, mdec.y);
                if (mdec.btn & 0x1) temp[1] = 'L';
                if (mdec.btn & 0x2) temp[2] = 'R';
                if (mdec.btn & 0x4) temp[3] = 'C';
                put_fonts8_asc(binfo->vram, binfo->scrn_x, 32, 16, COL8_FFFFFF, temp);

                m_x += mdec.x;
                m_y += mdec.y;

                if (m_x < 0) m_x = 0;
                if (m_y < 0) m_y = 0;
                if (m_x > binfo->scrn_x - 16) m_x = binfo->scrn_x - 16;
                if (m_y > binfo->scrn_y - 16) m_y = binfo->scrn_y - 16;

                sprintf(temp, "(%3d, %3d)", m_x, m_y);
                put_fonts8_asc(binfo->vram, binfo->scrn_x, 0, 0, COL8_FFFFFF, temp);

                put_block8_8(binfo->vram, binfo->scrn_x, 16, 16, m_x, m_y, mcursor, 16);
            }
        }
    }
    
    return 0;
}

