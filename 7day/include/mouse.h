#pragma once

#include "fifo.h"

#define KEYCMD_SENDTO_MOUSE         0xd4
#define MOUSECMD_ENABLE             0xf4

#define MOUSE_FIFO_BUF_SIZE         128

extern Fifo mousefifo;
extern unsigned char mousebuf[MOUSE_FIFO_BUF_SIZE];

void enable_mouse();