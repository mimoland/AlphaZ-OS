# 编译boot、loader和内核系统相关部分所用的规则
# boot和loader的编译输出.bin文件，输出目录为$(ROOTDIR)/build/，并
# 且该两项的头文件只在$(ROOTDIR)/arch/i386/include中，不依赖$(RO
# OTDIR)/include/中的头文件
#
# 其余文件会先编译成.o文件输出到$(ROOTDIR)/arch/i386/build,该目录
# 即下面的$(SUBBUILD)，最后将$(SUBBUILD)中的.o文件编译成静态库arch
# .o输出到$(ROOTDIR)/build供主Makefile使用

ASMBFLAGS += -I include/
ASMKFLAGS +=

CFLAGS 	  += -I $(ROOTDIR)/include

SUBBUILD   = build

BOOTOBJS	= 	$(BUILDDIR)/boot.bin \
				$(BUILDDIR)/loader.bin

OBJS		= $(SUBBUILD)/start.o	\
			  $(SUBBUILD)/liba.o \
			  $(SUBBUILD)/asm_start.o

TARGET		= $(BUILDDIR)/arch.a

.PHONY: all clear config

all: config $(BOOTOBJS) $(OBJS) $(TARGET)

$(BUILDDIR)/boot.bin : boot/boot.asm include/fat12header.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(BUILDDIR)/loader.bin : loader/loader.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(SUBBUILD)/start.o : kernel/start.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

$(SUBBUILD)/liba.o : lib/liba.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

$(SUBBUILD)/asm_start.o : kernel/asm_start.c
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

config:
	test -d $(SUBBUILD) || mkdir -p $(SUBBUILD)

clear:
	rm -f build/*
