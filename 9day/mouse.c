#include "fifo.h"
#include "io.h"
#include "keyboard.h"
#include "mouse.h"

Fifo mousefifo;
unsigned char mousebuf[MOUSE_FIFO_BUF_SIZE];

void enable_mouse(MouseDec* mdec)
{
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);

    mdec->phase = 0;
}

int mouse_decode(MouseDec* mdec, unsigned char dat)
{
    if (mdec->phase == 0)
    {
        // 等待鼠标的0xfa状态
        if (dat == 0xfa)
        {
            mdec->phase = 1;
        }
        return 0;
    }
    else if (mdec->phase == 1)
    {
        if ((dat & 0xc8) == 0x8)
        {
            // 第一个字节正确
            mdec->buf[0] = dat;
            mdec->phase = 2;
        }
        return 0;
    }
    else if (mdec->phase == 2)
    {
        // 第二个字节
        mdec->buf[1] = dat;
        mdec->phase = 3;
        return 0;
    }
    else if (mdec->phase == 3)
    {
        // 第三个字节
        mdec->buf[2] = dat;
        mdec->phase = 1;

        mdec->btn = mdec->buf[0] & 0x7;
        mdec->x = mdec->buf[1];
        mdec->y = mdec->buf[2];

        if ((mdec->buf[0] & 0x10) != 0)
        {
            mdec->x |= 0xffffff00;
        }
        if ((mdec->buf[0] & 0x20) != 0)
        {
            mdec->y |= 0xffffff00;
        }

        // 鼠标的y方向与画面符号相反
        mdec->y = -mdec->y;
        
        return 1;
    }
    return -1;
}