#pragma once

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

void init_palette(void);
void set_palette(int start, int end, unsigned char* rgb);
void box_fill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(unsigned char* vram, int x, int y);
void init_mouse_cursor8(char* mouse, char bc);
void put_block8_8(unsigned char* vram, int vsize_x, int psize_x, int psize_y, int px0, int py0, char* buf, int bsize_x);
void put_font8(unsigned char* vram, int size_x, int x, int y, char color, char* font);
void put_fonts8_asc(unsigned char* vram, int size_x, int x, int y, char color, char* s);