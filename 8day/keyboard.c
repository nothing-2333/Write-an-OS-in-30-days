#include "fifo.h"
#include "keyboard.h"
#include "io.h"

Fifo keyfifo;
unsigned char keybuf[KEY_FIFO_BUF_SIZE];

void wait_KBC_sendready()
{
    while (1)
    {
        if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) break;
    }
}

void init_keyboard()
{
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
}