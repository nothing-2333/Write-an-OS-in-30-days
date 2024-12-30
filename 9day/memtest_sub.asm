[BITS 32]

global memtest_sub

memtest_sub:                        ; unsigned int memtest_sub(unsigned int start, unsigned int end)
    push edi
    push esi 
    push ebx 
    mov esi, 0xaa55aa55
    mov edi, 0x55aa55aa
    mov eax, [esp + 12 + 4]
mts_loop:
    mov ebx, eax 
    add ebx, 0xffc
    mov edx, [ebx]                  ; 先记住修改前的值
    mov [ebx], esi                  ; 试写
    xor dword [ebx], 0xffffffff     ; 反转
    cmp edi, [ebx]
    jne mts_fin

    xor dword [ebx], 0xffffffff     ; 再次反转
    cmp esi, [ebx]
    jne mts_fin

    mov [ebx], edx 
    add eax, 0x1000
    cmp eax, [esp + 12 + 8]
    jbe mts_loop
    pop ebx 
    pop esi 
    pop edi 
    ret
mts_fin:
    mov [ebx], edx
    pop ebx 
    pop esi 
    pop edi 
    ret