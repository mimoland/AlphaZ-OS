
ASMBFLAGS += -I include/

OBJS	= 	$(BUILDDIR)/boot.bin \
			$(BUILDDIR)/loader.bin \
			$(BUILDDIR)/kernel.bin
.PHONY: all

all: $(OBJS)

$(BUILDDIR)/boot.bin : boot/boot.asm include/fat12header.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(BUILDDIR)/loader.bin : loader/loader.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(BUILDDIR)/kernel.bin : kernel/start.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<
