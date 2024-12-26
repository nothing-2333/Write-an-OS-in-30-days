void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char* rgb);
void box_fill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

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
#define COL8_005C8B 14
#define COL8_848484 15

// 入口函数HariMain重命名为标准的main
// 返回类型修改为int，避免编译器警告
int main(void)
{
    // 设定调色板
    init_palette();
    // 现存地址
    char* vram = (char*)0xa0000;

    int x_size = 320;
    int y_size = 200;

 
    box_fill8(vram, x_size, COL8_005C8B, 0, 0, x_size - 1, y_size - 29);
    box_fill8(vram, x_size, COL8_C6C6C6, 0, y_size - 28, x_size - 1, y_size - 28);
    box_fill8(vram, x_size, COL8_FFFFFF, 0, y_size - 27, x_size - 1, y_size - 27);
    box_fill8(vram, x_size, COL8_C6C6C6, 0, y_size - 26, x_size - 1, y_size - 1);

    box_fill8(vram, x_size, COL8_FFFFFF, 3, y_size - 24, 59, y_size - 24);
    box_fill8(vram, x_size, COL8_FFFFFF, 2, y_size - 24, 2, y_size - 4);
    box_fill8(vram, x_size, COL8_848484, 3, y_size - 4, 59, y_size - 4);
    box_fill8(vram, x_size, COL8_848484, 59, y_size - 23, 59, y_size - 5);
    box_fill8(vram, x_size, COL8_000000, 2, y_size - 4, 59, y_size - 3);
    box_fill8(vram, x_size, COL8_000000, 60, y_size - 24, 60, y_size - 3);

    box_fill8(vram, x_size, COL8_848484, x_size - 47, y_size - 24, x_size - 4, y_size - 24);
    box_fill8(vram, x_size, COL8_848484, x_size - 47, y_size - 23, x_size - 47, y_size - 4);
    box_fill8(vram, x_size, COL8_FFFFFF, x_size - 47, y_size - 3, x_size - 4, y_size - 3);
    box_fill8(vram, x_size, COL8_FFFFFF, x_size - 3, y_size - 24, x_size - 3, y_size - 3);

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