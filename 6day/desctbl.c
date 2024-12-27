#include "desctbl.h"
#include "int.h"

void init_gdt_idt(void)
{
    // gdt
    SegmentDescriptor* gdt = (SegmentDescriptor*)ADR_GDT;
    for (int i = 0; i <= LIMIT_GDT / 8; ++i)
    {
        set_segmdesc(gdt + i, 0, 0, 0);
    }
    // 描述符
    set_segmdesc(gdt + 1, 0xffffffff, 0x0, AR_DATA32_RW);
    set_segmdesc(gdt + 2, LIMIT_BOOTPACK, ADR_BOOTPACK, AR_CODE32_ER);
    load_gdtr(LIMIT_GDT, ADR_GDT);

    // idt
    GateDescriptor* idt = (GateDescriptor*)ADR_IDT;
    for (int i = 0; i <= LIMIT_IDT / 8; i++)
    {
        set_gatedesc(idt + i, 0, 0, 0);
    }
    // 描述符
    set_gatedesc(idt + 0x21, (int)asm_int_handler21, 2 << 3, AR_INTGATE32);
    set_gatedesc(idt + 0x27, (int)asm_int_handler27, 2 << 3, AR_INTGATE32);
    set_gatedesc(idt + 0x2c, (int)asm_int_handler2c, 2 << 3, AR_INTGATE32);
    load_idtr(LIMIT_IDT, ADR_IDT);
}

void set_segmdesc(SegmentDescriptor* sd, unsigned int limit, int base, int ar)
{
    if (limit > 0xfffff)
    {
        ar |= 0x8000;
        limit /= 0x1000;
    }
    sd->limit_low = limit & 0xffff;
    sd->base_low = base & 0xffff;
    sd->base_mid = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high = ((limit >> 16) & 0xf) | ((ar >> 8) & 0xf0);
    sd->base_high = (base >> 24) & 0xff;
    return ;
}

void set_gatedesc(GateDescriptor* gd, int offset, int selector, int ar)
{
    gd->offset_low = offset & 0xffff;
    gd->selector = selector;
    gd->dw_count = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high = (offset >> 16) & 0xffff;
    return ;
}