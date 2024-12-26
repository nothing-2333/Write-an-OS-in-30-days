all:
	nasm helloos.asm -o helloos.bin -l helloos.lst
	dd if=/dev/zero of=helloos.img bs=512 count=2880
	dd if=helloos.bin of=helloos.img bs=512 count=1 conv=notrunc

clean:
	rm -rf helloos.bin helloos.img helloos.lst

qemu: all
	qemu-system-i386 -drive file=helloos.img,format=raw,if=floppy

.PHONY:
	all