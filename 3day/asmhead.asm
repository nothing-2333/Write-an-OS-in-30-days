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
