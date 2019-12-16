
ASMBFLAGS += -I include/

OBJS	= 	$(BUILDDIR)/boot.bin \
			$(BUILDDIR)/loader.bin \
			$(BUILDDIR)/start.o
.PHONY: all

all: $(OBJS)

$(BUILDDIR)/boot.bin : boot/boot.asm include/fat12header.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(BUILDDIR)/loader.bin : loader/loader.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(BUILDDIR)/start.o : kernel/start.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<
