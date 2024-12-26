; 使用linker script指定起始地址
; org   0xc200            ; 程序被加载的内存地址

BOTPAK  EQU   0x00280000  ; 加载bootpack
DSKCAC  EQU   0x00100000  ; 磁盘缓存的位置
DSKCAC0 EQU   0x00008000  ; 实模式磁盘缓存的位置

; 有关BOOT_INFO
CYLS    EQU   0x0ff0      ; 设置启动区
LEDS    EQU   0x0ff1
VMODE   EQU   0x0ff2      ; 关于颜色数目的信息，颜色的位数
SCRNX   EQU   0x0ff4      ; 分辨率X
SCRNY   EQU   0x0ff6      ; 分辨率Y
VRAM    EQU   0x0ff8      ; 图像缓冲区的起始位置

[SECTION .btext]
[BITS 16]
entry:
    ; 设置屏幕模式
    mov al, 0x13        ; VGA显卡，320x200x8 bit
    mov ah, 0x0
    int 0x10
    ; 记录画面模式
    mov byte [VMODE], 8 ; 屏幕的模式
    mov word [SCRNX], 320
    mov word [SCRNY], 200
    mov dword [VRAM], 0xa0000   ; 显卡内存地址
    ; 用BIOS取得键盘上各种LED指示灯的状态
    mov ah, 0x02
    int 0x16            ; 键盘BIOS
    mov [LEDS], al

    ; 防止PIC接受所有中断
    ;   根据AT兼容机的规范初始化PIC
    ;   如果没有在CLI指令前执行可能会挂起
    ;   并继续初始化PIC
    mov al, 0xff
    out 0x21, al
    nop                     ; 有些机器不能连续执行NOP指令
    out 0xa1, al

    cli 

    ; 设置A20GATE使CPU支持1M以上的内存
    call waitkbdout
    mov al, 0xd1
    out 0x64, al 
    call waitkbdout
    mov al, 0xdf
    out 0x60, al 
    call waitkbdout

    ; 切换到保护模式
    ; NASM不支持INSTRSET命令
    ; [INSTRSET "i486p"]        ; 使用486指令
    lgdt [gdtr0]                ; 设置临时GDT
    mov eax, cr0                
    and eax, 0x7fffffff         ; 禁用分页
    or eax, 0x1                 ; 开启保护模式
    mov cr0, eax
    jmp pipelineflush

pipelineflush:
    ; 写32bit段
    mov ax, 1 * 8
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax
    ; bootpack传递
    mov esi, bootpack           ; 源
    mov edi, BOTPAK             ; 目标
    mov ecx, 512 * 1024 / 4
    call memcpy
    ; 传输磁盘数据
    ; 从引导区开始
    mov esi, 0x7c00             ; 源
    mov edi, DSKCAC             ; 目标
    mov ecx, 512 / 4
    call memcpy
    ; 剩余的全部
    mov esi, DSKCAC0 + 512      ; 源
    mov edi, DSKCAC + 512       ; 目标
    mov ecx, 0
    mov cl, byte[CYLS]
    imul ecx, 512 * 18 * 2 / 4  ; 除以4得到字节数
    sub ecx, 512 / 4            ; IPL偏移量
    call memcpy

    ; 由于还需要asmhead才能完成
    ; 完成其余的bootpack任务

    ; bootpack启动
    ; 修改后检验不通过，移除校验
    ; MOV   EBX, BOTPAK
    ; MOV   ECX, [EBX+16]
    ; ADD   ECX, 3            ; ECX += 3
    ; SHR   ECX, 2            ; ECX /= 4
    ; JZ    skip              ; 传输完成
    ; MOV   ESI, [EBX+20]     ; 源
    ; ADD   ESI, EBX
    ; MOV   EDI, [EBX+12]     ; 目标
    ; CALL  memcpy

skip:
    ; 堆栈初始化
    ; MOV   ESP, [EBX+12]     ; 堆栈初始化
    ; JMP   DWORD 2*8:0x0000001b
    mov esp, 0xffff
    jmp dword 2*8:0x0

waitkbdout:
    in al, 0x64
    and al, 0x2
    jnz waitkbdout              ; AND结果不为0跳转至waitkbdout
    ret

memcpy:
    mov eax, [esi]
    add esi, 4
    mov [edi], eax
    add edi, 4
    sub ecx, 1
    jnz memcpy                  ; 结果不为0跳转至memcpy
    ret

ALIGN 16
gdt0:
    resb 8
    dw 0xffff, 0x0, 0x9200, 0xcf
    dw 0xffff, 0x0, 0x9a28, 0x0047

    dw 0

gdtr0:
    dw 8 * 3 - 1
    dd gdt0
ALIGN 16

; 后边会被链接c编译后的汇编
bootpack:
