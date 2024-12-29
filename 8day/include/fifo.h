#pragma once

#define FLAGS_OVERRUN 0x0001

typedef struct _Fifo
{
    unsigned char* buf;
    int size, free, flags;
    int next_r, next_w; // next_r正要读的，next_w是正要写的
} Fifo;

void fifo8_init(Fifo* fifo, int size, unsigned char* buf);
int fifo8_put(Fifo* fifo, unsigned char data);
int fifo8_get(Fifo* fifo);
int fifo8_status(Fifo* fifo);