
ROOTDIR		= $(shell pwd)

MAKE		= 	make
ASM			= 	nasm
DASM		=	ndisasm
CC			= 	gcc
LD			= 	LD
ASMBFLAGS	= 				# boot编译选项
ASMKFLAGS	=				# kernel编译选项
CFLAGS		=
LDFLAGS		=
DASMFLAGS	=

ARCH		= i386
BUILDDIR 	= $(ROOTDIR)/build
SUBDIRS		= arch/$(ARCH)

TARGET		=

# 导出子目录中需要用到的变量
export ROOTDIR
export ASM DASM CC LD ASMBFLAGS ASMKFLAGS CFLAGS LDFLAGS DASMFLAGS
export ARCH BUILDDIR SUBDIRS

.PHONY: debug all buildimg image

all: $(SUBDIRS)

buildimg :
	dd if=$(BUILDDIR)/boot.bin of=a.img bs=512 count=1 conv=notrunc
	# sudo mount -o loop a.img /mnt/floppy/
	# sudo cp -fv $(BUILDDIR)/* /mnt/floppy/
	# sudo umount /mnt/floppy

image: all buildimg

debug:
	@echo $(ROOTDIR)
	@echo $(SUBDIRS)


$(SUBDIRS): ECHO
	$(MAKE) -C $@

ECHO:
	@echo =======================$(SUBDIRS)============================
