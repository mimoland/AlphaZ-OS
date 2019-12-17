
# kernel入口地址
ENTRYPOINT = 0x30400

ROOTDIR		= $(shell pwd)

MAKE		= 	make
ASM			= 	nasm
DASM		=	ndisasm
CC			= 	gcc
LD			= 	ld
AR 			=	ar

# boot编译选项
ASMBFLAGS	=

# kernel编译选项
ASMKFLAGS	=	-f elf

CFLAGS		=	-I include/ -c -fno-builtin -fno-common -m32 -fno-stack-protector
LDFLAGS		=   -m elf_i386 -s -Ttext $(ENTRYPOINT)
ARFLAGS		=	-rc
DASMFLAGS	=

ARCH		= i386
BUILDDIR 	= $(ROOTDIR)/build
SUBDIRS		= arch/$(ARCH)

TARGET		=	$(BUILDDIR)/kernel.bin
SLIBS		=	$(BUILDDIR)/arch.a

# 导出子目录中需要用到的变量
export ROOTDIR
export ASM DASM CC LD AR ASMBFLAGS ASMKFLAGS CFLAGS LDFLAGS DASMFLAGS ARFLAGS
export ARCH BUILDDIR SUBDIRS

.PHONY: debug all buildimg image clear config

all: config $(SUBDIRS) $(TARGET)

buildimg :
	dd if=$(BUILDDIR)/boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy/
	sudo cp -fv $(BUILDDIR)/loader.bin /mnt/floppy/
	sudo cp -fv $(BUILDDIR)/kernel.bin /mnt/floppy/
	sudo umount /mnt/floppy

image: all buildimg

clear:
	$(MAKE) clear -C $(SUBDIRS)
	@echo 'delete the objects which in build dir'
	rm -f $(BUILDDIR)/*
	@echo 'delete the linked include/asm'
	rm include/asm

$(TARGET): $(SLIBS)
	$(LD) $(LDFLAGS) -o $@ $^

debug:
	@echo $(ROOTDIR)
	@echo $(SUBDIRS)

# make之前对项目进行一些配置
config:
	ln -fsn $(ROOTDIR)/include/asm-$(ARCH) include/asm
	@echo	'/include/asm-$(ARCH) had linked to include/asm'


$(SUBDIRS): ECHO
	$(MAKE) -C $@

ECHO:
	@echo =======================$(SUBDIRS)============================
