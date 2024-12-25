all:
	nasm helloos.asm -o helloos.bin

clean:
	rm -rf helloos.bin

qemu: all
	qemu-system-i386 -drive file=helloos.bin,format=raw

.PHONY:
	all