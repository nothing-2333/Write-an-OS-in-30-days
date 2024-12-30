#include "graphic.h"
#include "io.h"

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