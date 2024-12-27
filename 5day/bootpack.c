#include <stdio.h>

#include "io.h"
#include "pm.h"


#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_00477D 14
#define COL8_848484 15

typedef struct _BootInfo
{
    char cyls, leds, vmode, reserve;
    short scrn_x, scrn_y;
    unsigned char* vram;
} BootInfo;

typedef struct _SegmentDescriptor
{
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
} SegmentDescriptor;

typedef struct _GateDescriptor
{
    
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
} GateDescriptor;

// func.asm
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
// gui
void init_palette(void);
void set_palette(int start, int end, unsigned char* rgb);
void box_fill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(unsigned char* vram, int x, int y);
void init_mouse_cursor8(char* mouse, char bc);
void put_block8_8(unsigned char* vram, int vsize_x, int psize_x, int psize_y, int px0, int py0, char* buf, int bsize_x);
void put_font8(unsigned char* vram, int size_x, int x, int y, char color, char* font);
void put_fonts8_asc(unsigned char* vram, int size_x, int x, int y, char color, char* s);

// 系统
void init_gdt_idt(void);
void set_segmdesc(SegmentDescriptor* sd, unsigned int limit, int base, int ar);
void set_gatedesc(GateDescriptor* gd, int offset, int selector, int ar);

// 入口函数HariMain重命名为标准的main
// 返回类型修改为int，避免编译器警告
int main(void)
{
    BootInfo *binfo = (BootInfo*)0xff0;
    char mcursor[256];

    // 设定调色板
    init_palette();

    // 初始化gdt、idt
    init_gdt_idt();
 
    init_screen(binfo->vram, binfo->scrn_x, binfo->scrn_y);


    // 鼠标初始位置
    int m_x = (binfo->scrn_x - 16) / 2;
    int m_y = (binfo->scrn_y - 28 - 16) / 2;
    init_mouse_cursor8(mcursor, COL8_00477D);
    put_block8_8(binfo->vram, binfo->scrn_x, 16, 16, m_x, m_y, mcursor, 16);

    // 显示字
    put_fonts8_asc(binfo->vram, binfo->scrn_x, 8, 8, COL8_FFFFFF, "ABC 123");
    put_fonts8_asc(binfo->vram, binfo->scrn_x, 31, 31, COL8_000000, "Haribote OS.");
    put_fonts8_asc(binfo->vram, binfo->scrn_x, 30, 30, COL8_FFFFFF, "Haribote OS.");

    char s[40];
    sprintf(s, "nothing_%d", 2333);
    put_fonts8_asc(binfo->vram, binfo->scrn_x, 16, 64, COL8_FFFFFF, s);

    for (;;) io_hlt();
    return 0;
}

void init_palette(void)
{
    static unsigned char table_rgb[3 * 16] = {
        0x00, 0x00, 0x00, // 黑色
        0xff, 0x00, 0x00, // 亮红色
        0x00, 0xff, 0x00, // 亮绿色
        0xff, 0xff, 0x00, // 亮黄色
        0x00, 0x00, 0xff, // 亮蓝色
        0xff, 0x00, 0xff, // 亮紫色
        0x00, 0xff, 0xff, // 浅亮蓝色
        0xff, 0xff, 0xff, // 白色
        0xc6, 0xc6, 0xc6, // 亮灰色
        0x84, 0x00, 0x00, // 暗红色
        0x00, 0x84, 0x00, // 暗绿色
        0x84, 0x84, 0x00, // 暗黄色
        0x00, 0x00, 0x84, // 暗蓝色
        0x84, 0x00, 0x84, // 暗紫色
        0x00, 0x47, 0x7d, // 浅暗蓝色
        0x84, 0x84, 0x84  // 暗灰色
    };

    set_palette(0, 15, table_rgb);

    return;
}

// 设置调色板，叫做重定义掉色版更恰当些
void set_palette(int start, int end, unsigned char* rgb)
{
    int eflags = io_load_eflags();

    // 禁止中断
    io_cli();

    io_out8(0x3c8, start);
    for (int i = start; i <= end; ++i)
    {
        io_out8(0x3c9, rgb[0] / 4);
        io_out8(0x3c9, rgb[1] / 4);
        io_out8(0x3c9, rgb[2] / 4);
        rgb += 3;
    }

    io_store_eflags(eflags);

    return ;
} 

void box_fill8(unsigned char* vram, int x_size, unsigned char color, int x0, int y0, int x1, int y1)
{
    for (int y = y0; y <= y1; y++)
    {
        for (int x = x0; x <= x1; x++)
        {
            vram[y * x_size + x] = color;
        }
    }

    return ;
}

void init_screen(unsigned char* vram, int size_x, int size_y)
{
    box_fill8(vram, size_x, COL8_00477D, 0, 0, size_x - 1, size_y - 29);
    box_fill8(vram, size_x, COL8_C6C6C6, 0, size_y - 28, size_x - 1, size_y - 28);
    box_fill8(vram, size_x, COL8_FFFFFF, 0, size_y - 27, size_x - 1, size_y - 27);
    box_fill8(vram, size_x, COL8_C6C6C6, 0, size_y - 26, size_x - 1, size_y - 1);

    box_fill8(vram, size_x, COL8_FFFFFF, 3, size_y - 24, 59, size_y - 24);
    box_fill8(vram, size_x, COL8_FFFFFF, 2, size_y - 24, 2, size_y - 4);
    box_fill8(vram, size_x, COL8_848484, 3, size_y - 4, 59, size_y - 4);
    box_fill8(vram, size_x, COL8_848484, 59, size_y - 23, 59, size_y - 5);
    box_fill8(vram, size_x, COL8_000000, 2, size_y - 4, 59, size_y - 3);
    box_fill8(vram, size_x, COL8_000000, 60, size_y - 24, 60, size_y - 3);

    box_fill8(vram, size_x, COL8_848484, size_x - 47, size_y - 24, size_x - 4, size_y - 24);
    box_fill8(vram, size_x, COL8_848484, size_x - 47, size_y - 23, size_x - 47, size_y - 4);
    box_fill8(vram, size_x, COL8_FFFFFF, size_x - 47, size_y - 3, size_x - 4, size_y - 3);
    box_fill8(vram, size_x, COL8_FFFFFF, size_x - 3, size_y - 24, size_x - 3, size_y - 3);
}

void init_mouse_cursor8(char* mouse, char bc)
{
    static char cursor[16][16] = {
        "**************..",
        "*OOOOOOOOOOO*...",
        "*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....",
        "*OOOOOOOO*......",
        "*OOOOOOO*.......",
        "*OOOOOOO*.......",
        "*OOOOOOOO*......",
        "*OOOO**OOO*.....",
        "*OOO*..*OOO*....",
        "*OO*....*OOO*...",
        "*O*......*OOO*..",
        "**........*OOO*.",
        "*..........*OOO*",
        "............*OO*",
        ".............***"
    };

    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            if (cursor[y][x] == '*') mouse[y * 16 + x] = COL8_000000;

            if (cursor[y][x] == 'O') mouse[y * 16 + x] = COL8_FFFFFF;

            if (cursor[y][x] == '.') mouse[y * 16 + x] = bc;
        }
    }
}

void put_block8_8(unsigned char* vram, int vsize_x, int psize_x, int psize_y, int px0, int py0, char* buf, int bsize_x)
{
    for (int y = 0; y < psize_y; ++y)
    {
        for (int x = 0; x < psize_x; ++x)
        {
            vram[(py0 + y) * vsize_x + (px0 + x)] = buf[y * bsize_x + x];
        }
    }
}

void put_font8(unsigned char* vram, int size_x, int x, int y, char color, char* font)
{
    unsigned char* p;
    char d;

    for (int i = 0; i < 16; i++)
    {
        p = vram + (y + i) * size_x + x;
        d = font[i];
        int left = 0x80;
        for (int j = 0; j < 8; j++)
        {
            if ((d & left) != 0) p[j] = color;
            left = left / 2;
        }
    }
    
}

void put_fonts8_asc(unsigned char* vram, int size_x, int x, int y, char color, char* s)
{
    extern char hankaku[4096];

    for (; *s != '\0'; s++)
    {
        put_font8(vram, size_x, x, y, color, hankaku + *s * 16);
        x += 8;
    }
}

void init_gdt_idt(void)
{
    SegmentDescriptor* gdt = (SegmentDescriptor*)0x270000;
    GateDescriptor* idt = (GateDescriptor*)0x26f800;

    // 初始化为0
    for (int i = 0; i < 8192; ++i)
    {
        set_segmdesc(gdt + i, 0, 0, 0);
    }
    // 0x4092是段描述符的属性
    set_segmdesc(gdt + 1, 0xffffffff, 0x0, 0x4092);
    set_segmdesc(gdt + 2, 0x7ffff, 0x280000, 0x409a);
    load_gdtr(0xffff, 0x270000);

    for (int i = 0; i < 256; i++)
    {
        set_gatedesc(idt + i, 0, 0, 0);
    }
    load_idtr(0x7ff, 0x26f800);
}

void set_segmdesc(SegmentDescriptor* sd, unsigned int limit, int base, int ar)
{
    if (limit > 0xfffff)
    {
        ar |= 0x8000;
        limit /= 0x1000;
    }
    sd->limit_low = limit & 0xffff;
    sd->base_low = base & 0xffff;
    sd->base_mid = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high = ((limit >> 16) & 0xf) | ((ar >> 8) & 0xf0);
    sd->base_high = (base >> 24) & 0xff;
    return ;
}

void set_gatedesc(GateDescriptor* gd, int offset, int selector, int ar)
{
    gd->offset_low = offset & 0xffff;
    gd->selector = selector;
    gd->dw_count = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high = (offset >> 16) & 0xffff;
    return ;
}