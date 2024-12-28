#pragma once

#define ADR_BOOTINFO 0x00000ff0

typedef struct _BootInfo
{
    char cyls, leds, vmode, reserve;
    short scrn_x, scrn_y;
    unsigned char* vram;
} BootInfo;