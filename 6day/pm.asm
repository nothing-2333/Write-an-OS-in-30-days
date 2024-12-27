[BITS 32]

global load_gdtr, load_idtr

[SECTION .text]

load_gdtr:              ; void load_gdtr(int limit, int address);
    mov ax, [esp + 4]
    mov [esp + 6], ax 
    lgdt [esp + 6]
    ret

load_idtr:              ; void load_idtr(int limit, int address);
    mov ax, [esp + 4]
    mov [esp + 6], ax
    lidt [esp + 6]
    ret 