#include "mem.h"
#include "io.h"


unsigned int memtest(unsigned int start, unsigned int end)
{
    char flg486 = 0;
    unsigned int eflg, cr0;

    // 确定cpu是486以上还是386
    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT;
    io_store_eflags(eflg);
    eflg = io_load_eflags();
    if (eflg & EFLAGS_AC_BIT) flg486 = 1;
    // 还原
    eflg &= ~EFLAGS_AC_BIT;
    io_store_eflags(eflg);

    // 禁止缓存
    if (flg486)
    {
        cr0 = io_load_cr0();
        cr0 |= CR0_CACHE_DISABLE;
        io_store_cr0(cr0);
    }

    unsigned int ret = memtest_sub(start, end);

    // 允许缓存
    if (flg486)
    {
        cr0 = io_load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;
        io_store_cr0(cr0);
    }

    return ret;
}

void memman_init(MemMan* man)
{
    // 可用信息数目
    man->frees = 0;
    // 用于观察可用状况：frees的最大值
    man->maxfrees = 0;
    // 释放失败的内存的大小总和
    man->lostsize = 0;
    // 释放失败次数
    man->losts = 0;
}

unsigned int memman_total(MemMan* man)
{   
    unsigned int t = 0;
    for (unsigned int i = 0; i < man->frees; ++i)
    {
        t += man->free[i].size;
    }
    return t;
}   

unsigned int memman_alloc(MemMan* man, unsigned int size)
{
    unsigned int a;
    for (unsigned int i = 0; i < man->frees; ++i)
    {
        if (man->free[i].size >= size)
        {
            a = man->free[i].addr;
            man->free[i].addr += size;
            man->free[i].size -= size;

            if (man->free[i].size == 0)
            {
                man->frees--;
                for (; i < man->frees; ++i)
                {
                    man->free[i] = man->free[i + i];
                }
            }
            return a;
        }
    }
    return 0;
}

int memman_free(MemMan* man, unsigned int addr, unsigned int size)
{
    int i;

    // 找到前一项
    for (i = 0; i < man->frees; i++)
    {
        if (man->free[i].addr > addr) break;
    }
    
    // 可以与前面的可用内存归纳到一起
    if (i > 0)
    {
        if (man->free[i - 1].addr + man->free[i - 1].size == addr)
        {
            man->free[i - 1].size += size;
        }

        // 也可以与后面的可用内存归纳到一起
        if (i < man->frees)
        {
            if (addr + size == man->free[i].addr)
            {
                man->free[i - 1].size += man->free[i].size;
                man->frees--;
                for(; i < man->frees; ++i)
                {
                    man->free[i] = man->free[i + 1];
                }
            }
        }
    }

    // 不能与前面的可用空间归纳到一起,可以与后面的可用内存归纳到一起
    if (i < man->frees)
    {
        if (addr + size == man->free[i].addr)
        {
            man->free[i].addr = addr;
            man->free[i].size += size;
            return 0;
        }
    }

    // 既不能与前面归纳到一起，也不能与后面归纳到一起
    if (man->frees < MEMMAN_FREES)
    {
        // free[i]之后的，向后移动，腾出free[i]
        for (int j = man->frees; j > i; --j)
        {
            man->free[j] = man->free[j - 1];
        }

        man->frees++;

        // 更新最大值
        if (man->maxfrees < man->frees)
        {
            man->maxfrees = man->frees;
        }

        man->free[i].addr = addr;
        man->free[i].size =size;
        return 0;
    }

    // 不能往后移动
    man->losts++;
    man->lostsize += size;
    return -1;
}
