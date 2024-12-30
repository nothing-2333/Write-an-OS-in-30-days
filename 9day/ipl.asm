CYLS   EQU   10               ; 读取的柱面数量（CYLS = cylinders）

org 0x7c00

; 标准FAT12格式软盘专用代码
jmp entry
DB    0x90              ; NOP指令
DB    "HELLOIPL"        ; OEM标识符（8字节）
DW    512               ; 每个扇区（sector）的字节数（必须为512字节）
DB    1                 ; 每个簇（cluster）的扇区数（必须为1个扇区）
DW    1                 ; FAT的预留扇区数（包含boot扇区）
DB    2                 ; FAT表的数量，通常为2
DW    224               ; 根目录文件的最大值（一般设为224项）
DW    2880              ; 磁盘的扇区总数，若为0则代表超过65535个扇区，需要使用19行记录
DB    0xf0              ; 磁盘的种类（本项目中设为0xf0代表1.44MB的软盘）
DW    9                 ; 每个FAT的长度（必须为9扇区）
DW    18                ; 1个磁道（track）拥有的扇区数（必须是18）
DW    2                 ; 磁头数（必须为2）
DD    0                 ; 隐藏的扇区数
DD    2880              ; 大容量扇区总数，若13行记录的值为0则使用本行记录扇区数
DB    0                 ; 中断0x13的设备号
DB    0                 ; Windows NT标识符
DB    0x29              ; 扩展引导标识
DD    0xffffffff        ; 卷序列号
DB    "HELLO-OS   "     ; 卷标（11字节）
DB    "FAT12   "        ; 文件系统类型（8字节）

; 引导代码、数据以及其它信息
RESB  18                ; 空18字节


; 程序主体
entry:
    ; 初始化寄存器
    mov ax, 0
    mov ss, ax
    mov sp, 0x7c00
    mov ds, ax 

    ; 读取硬盘
    mov ax, 0x820
    mov es, ax
    mov ch, 0           ; 柱面0
    mov dh, 0           ; 磁头0
    mov cl, 2           ; 扇区2

readloop:
    mov si, 0           ; 记录失败次数的寄存器

retry:
    mov ah, 0x2        ; 读盘
    mov al, 1           ; 1个扇区
    mov bx, 0
    mov dl, 0x0         ; A驱动器
    int 0x13            ; 调用磁盘BIOS
    jnc next             ; 没出错跳转到next
    add si, 1           ; 失败次数+1
    cmp si, 5           ; 失败次数是否达到5次
    jae error           ; 失败次数达到5次跳转到error
    mov ah, 0x0
    mov dl, 0x0         ; A驱动器
    int 0x13            ; 重置驱动器
    jmp retry

next:
    ; 把内存地址后移0x200
    mov ax, es       
    add ax, 0x20
    mov es, ax          ; 实现ES += 0x0020的目的
    ; 扇区范围 1～18
    add cl, 1           ; 扇区加1
    cmp cl, 18          ; 扇区是否达到18
    jbe readloop        ; 小于等于18扇区则跳转到readloop
    mov cl, 1           ; 恢复到扇区1
    ; 磁头范围 0～1（正面0，反面1）
    add dh, 1
    cmp dh, 2
    jb readloop         ; 磁头未达到2则跳转到readloop
    ; 柱面范围 0 ～ 79
    mov dh, 0
    add ch, 1
    cmp ch, CYLS
    jb readloop         ; 读取指定数量的柱面
    ; 磁盘 10 * 2 * 18 * 512 - 512 = 180kb - 512b 填充到了0x8200-0x34fff
    ; 读取完毕，跳转到haribote.sys执行
    mov [0xff0], ch     ; 记下IPL读了多远
    jmp 0xc200

fin: 
    ; 结束
    hlt             ; 让CPU停止，等待指令
    jmp fin

error:
    mov si, msg

putloop:
    mov al, [si]
    add si, 1
    cmp al, 0

    je fin
    mov ah, 0x0e    ; 显示一个文字
    mov bx, 15      ; 指定字符颜色
    int 0x10        ; 调用显卡BIOS 
    jmp putloop

msg:
    db 0x0a, 0x0a   ; 换行2次
    db "load, error"
    db 0x0a
    db 0

times  0x1fe - ($ - $$) db 0
DB    0x55, 0xaa
