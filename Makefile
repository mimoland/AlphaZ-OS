
# kernel入口地址
ENTRYPOINT = 0x30400

MAKEFLAGS  += -rR --no-print-directory

ASM			= nasm
DASM		= ndisasm
CC			= gcc
LD			= ld
AR 			= ar

# boot编译选项
ASMBFLAGS	=

# kernel编译选项
ASMKFLAGS	= -f elf
CFLAGS		= -I include/ -c -fno-builtin -fno-common -m32 -fno-stack-protector
LDFLAGS		= -m elf_i386 -s -Ttext $(ENTRYPOINT)
ARFLAGS		= -rc
DASMFLAGS	=
PHONY		:=

export ASM DASM CC LD AR ASMBFLAGS ASMKFLAGS CFLAGS LDFLAGS ARFLAGS DASMFLAGS PHONY

# 欲生成平台的处理器架构
ARCH		?= i386
export ARCH

srctree		:= $(CURDIR)
builddir 	:= $(srctree)/build
export srctree builddir

alphaz  := $(builddir)/kernel.bin

boot-bin :=

# 与体系相关的obj文件和静态库文件，arch-obj会在arch/$(ARCH)/Makefile中添加新成员
arch-obj :=
arch-lib := $(builddir)/arch.a

subdir 	:= arch/$(ARCH)/

_all: all
PHONY += _all


all: config $(alphaz)
PHONY += all

buildimg :
	dd if=$(builddir)/boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy/
	sudo cp -fv $(builddir)/loader.bin /mnt/floppy/
	sudo cp -fv $(builddir)/kernel.bin /mnt/floppy/
	sudo umount /mnt/floppy
PHONY += buildimg

image: all buildimg
PHONY += image

clear:
	rm -f $(builddir)/*
	test -d include/asm && rm include/asm
PHONY += clear

include arch/$(ARCH)/Makefile

all: $(boot-bin)
PHONY += all

$(arch-lib) : $(arch-obj)
	$(AR) $(ARFLAGS) $@ $^

$(alphaz): $(arch-lib)
	$(LD) $(LDFLAGS) -o $@ $^

debug:
	@echo $(srctree)
	@echo $(subdir)
	@echo $(boot-bin)
	@echo $(arch-obj)
PHONY += debug

# make之前对项目进行一些配置
config:
	ln -fsn $(srctree)/include/asm-$(ARCH) include/asm
	test -d $(builddir) || mkdir -p $(builddir)

.PHONY = $(PHONY)
