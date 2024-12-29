#pragma once

#include "fifo.h"

#define KEYCMD_SENDTO_MOUSE         0xd4
#define MOUSECMD_ENABLE             0xf4

#define MOUSE_FIFO_BUF_SIZE         128

typedef struct _MouseDec
{
    unsigned char buf[3], phase;
    int x, y, btn;
} MouseDec;

extern Fifo mousefifo;
extern unsigned char mousebuf[MOUSE_FIFO_BUF_SIZE];

void enable_mouse(MouseDec* mdec);
int mouse_decode(MouseDec* mdec, unsigned char dat);