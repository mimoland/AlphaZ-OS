
# kernel入口地址
ENTRYPOINT = 0x30400

ROOTDIR		= $(shell pwd)

MAKE		= 	make
ASM			= 	nasm
DASM		=	ndisasm
CC			= 	gcc
LD			= 	ld

# boot编译选项
ASMBFLAGS	=

# kernel编译选项
ASMKFLAGS	=	-f elf

CFLAGS		=
LDFLAGS		=   -m elf_i386 -s -Ttext $(ENTRYPOINT)
DASMFLAGS	=

ARCH		= i386
BUILDDIR 	= $(ROOTDIR)/build
SUBDIRS		= arch/$(ARCH)

TARGET		=	$(BUILDDIR)/kernel.bin
OBJS	=	$(BUILDDIR)/start.o

# 导出子目录中需要用到的变量
export ROOTDIR
export ASM DASM CC LD ASMBFLAGS ASMKFLAGS CFLAGS LDFLAGS DASMFLAGS
export ARCH BUILDDIR SUBDIRS

.PHONY: debug all buildimg image clear

all: $(SUBDIRS) $(TARGET)

buildimg :
	dd if=$(BUILDDIR)/boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy/
	sudo cp -fv $(BUILDDIR)/loader.bin /mnt/floppy/
	sudo cp -fv $(BUILDDIR)/kernel.bin /mnt/floppy/
	sudo umount /mnt/floppy

image: all buildimg

clear:
	rm -f $(BUILDDIR)/*

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

debug:
	@echo $(ROOTDIR)
	@echo $(SUBDIRS)


$(SUBDIRS): ECHO
	$(MAKE) -C $@

ECHO:
	@echo =======================$(SUBDIRS)============================
