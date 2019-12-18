

ASMBFLAGS += -I arch/i386/include/
ASMKFLAGS +=

subarch := arch/$(ARCH)

boot-bin += $(builddir)/boot.bin \
			$(builddir)/loader.bin

arch-obj += $(builddir)/start.o	\
			$(builddir)/liba.o \
			$(builddir)/asm_start.o

$(builddir)/boot.bin : $(subarch)/boot/boot.asm arch/$(ARCH)/include/fat12header.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(builddir)/loader.bin : $(subarch)/loader/loader.asm arch/$(ARCH)/include/fat12header.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(builddir)/start.o : $(subarch)/kernel/start.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

$(builddir)/liba.o : $(subarch)/lib/liba.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

$(builddir)/asm_start.o : $(subarch)/kernel/asm_start.c
	$(CC) $(CFLAGS) -o $@ $<

