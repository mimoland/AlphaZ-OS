
ASMBFLAGS += -I include/

$(BUILDDIR)/boot.bin : boot/boot.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<
