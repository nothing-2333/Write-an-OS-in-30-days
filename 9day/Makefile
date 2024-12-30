# 编译配置
ifndef GCCPREFIX
GCCPREFIX :=
endif
AS			:= nasm
CC			:= $(GCCPREFIX)gcc 
LD			:= $(GCCPREFIX)ld 
OBJCOPY		:= $(GCCPREFIX)objcopy
QEMU		:= qemu-system-i386

# 附加命令
# gcc
CFLAGS 		+= -Wall
CFLAGS 		+= -std=gnu99 -static -m32
CFLAGS 		+= -I ./libc/include -I ./include
CFLAGS 		+= -ffunction-sections -nostdlib -nostdinc -fno-builtin -ffreestanding
CFLAGS 		+= -fno-pie
# qemu
QEMU_FLAGS 	+= -no-reboot -d in_asm
ifdef DEBUG
QEMU_FLAGS 	+= -gdb tcp::1234 -S
CFLAGS 		+= -g
endif

# 编译目标
# C Library Objects
L_OBJS		:= libc/stdio/stdio.bin
# Kernel Objects
K_OBJS		:= bootpack.bin io.bin pm.bin hankaku.bin desctbl.bin graphic.bin int.bin inthandler.bin fifo.bin keyboard.bin mouse.bin mem.bin memtest_sub.bin
SYS 		:= haribote.sys  
IMG			:= haribote.img

# 启动
ipl.bin:
	$(AS) -f bin ipl.asm -o ipl.bin -l ipl.lst

asmhead.bin:
	$(AS) -f bin asmhead.asm -o asmhead.bin -l asmhead.lst

%.bin: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

%.bin: %.asm 
	$(AS) -f elf $< -o $@ -l $(subst .asm,.lst,$<)

kernel.sys: $(L_OBJS) $(K_OBJS)
	$(LD) -m elf_i386 --oformat binary -o kernel.sys -T kernel.lds $^

haribote.sys: asmhead.bin kernel.sys
	cat asmhead.bin > haribote.sys
	cat kernel.sys >> haribote.sys

# 加载到img的0x4200地址处，在内存中 0x8000 + 0x4200 = 0xc200
image: ipl.bin haribote.sys
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=ipl.bin of=$(IMG) bs=512 conv=notrunc
	dd if=haribote.sys of=$(IMG) seek=33 bs=512 conv=notrunc	

all: $(OBJS) haribote.sys image

clean:
	rm -rf *.bin
	rm -rf *.sys
	rm -rf *.obj
	rm -rf *.lst
	rm -rf **/**/*.bin
	rm -rf $(IMG)

qemu: clean all 
	$(QEMU) -drive file=$(IMG),format=raw,if=floppy $(QEMU_FLAGS)

.PHONY: all