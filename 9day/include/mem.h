#pragma once

#define MEMMAN_ADDR         0x003c0000
#define MEMMAN_FREES        4090 

typedef struct _FreeInfo
{
    unsigned int addr, size;
} FreeInfo;

typedef struct _MemMan
{
    int frees, maxfrees, lostsize, losts;
    FreeInfo free[MEMMAN_FREES];
} MemMan;

unsigned int memtest(unsigned int start, unsigned int end);
unsigned int memtest_sub(unsigned int start, unsigned int end);

void memman_init(MemMan* man);
unsigned int memman_total(MemMan* man);
unsigned int memman_alloc(MemMan* man, unsigned int size);
int memman_free(MemMan* man, unsigned int addr, unsigned int size);
