# 跟着敲系列
跟着《30天自制操作系统》（川合秀实 著）实现一个操作系统，使用NASM、GCC、Qemu等工具进行编译、运行。

# 代码部分参考的这个项目，大部分名字都沿用了，我实在不知道起什么名字好，另外这个项目作者太厉害了，在看他的代码的时候感觉学到了很多新的知识，除了一些已经熟练掌握部分复制了一下（应该只有1day那部分...），其他全部动手敲了一遍，另外在说明一点是，这个老项目在新版本中有些已经不能用了，我更新这些，有些地方在他原有的代码上做出了改进.共勉!
https://gitee.com/ghosind/HariboteOS/tree/main

## 有些命令怕忘记，写在这里了

sudo apt install qemu-system-x86

sudo make qemu

## 有些我在做这个新学到的知识也复制到这里
### Makefile 中 .PHONY
在 Makefile 中使用 .PHONY 声明的目标被视为“伪目标”，这意味着,总是执行命令 && 无视文件存在
### dd if=/dev/zero of=helloos.img bs=512 count=2880
命令 dd if=/dev/zero of=helloos.img bs=512 count=2880 使用了 dd 工具来创建一个磁盘镜像文件。下面是该命令各部分的详细说明：

dd：这是 Linux 和 Unix 系统中用于转换和复制文件的一个命令行工具，它特别适用于低级操作，比如从设备读取数据或向设备写数据。

if=/dev/zero：if 指定输入文件（input file）。/dev/zero 是一个特殊的字符设备文件，它提供无限的零（0）。当 dd 从 /dev/zero 读取时，它会得到一个无限的零流。

of=helloos.img：of 指定输出文件（output file）。在这个命令中，helloos.img 是将要创建的文件名，dd 将把从 /dev/zero 读取的数据写入这个文件。

bs=512：bs 代表块大小（block size）。这里设置为 512 字节，意味着 dd 在读取和写入数据时将以 512 字节为单位进行操作。这是磁盘扇区的标准大小，因此这个设置使得创建的镜像文件可以被当作磁盘扇区来处理。

count=2880：count 指定要复制的块的数量。这里设置为 2880，意味着 dd 将从 /dev/zero 读取 2880 个 512 字节的块，总共 1440000 字节（1.44 MB）。这个大小常用于模拟一个 1.44 MB 的软盘。
### dd 中 conv=notrunc 选项
conv=notrunc 是 dd 命令的一个转换（conv）选项，它告诉 dd 不要截断输出文件。换句话说，如果输出文件已经存在，dd 将不会将其大小缩减到指定的 count 和 bs（块大小）的乘积。相反，它会保留文件的原始大小，只在文件的开始处写入指定的数据。
### gdb 查看汇编指令 display /20i $pc display /20i 0x7c00
### 将显示的汇编指令转换为intel格式的汇编 set disassembly-flavor intel (默认是: set disassembly-flavor att )
### cyls EQU 10
在汇编语言中，EQU 是一个伪指令，用于定义一个符号常量。EQU 指令将一个值或表达式与一个名称关联起来，这样在程序中就可以使用这个名称来代替实际的值。每次出现 cyls 时，汇编器都会将其替换为数值 10。