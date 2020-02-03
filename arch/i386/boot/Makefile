
target-dir = ../../../target

ASM := nasm
ASMFLAGS :=

all: $(target-dir)/boot.bin $(target-dir)/loader.bin

$(target-dir)/boot.bin: boot.asm fat12header.inc
	$(ASM) $(ASMFLAGS) -o $@ $<

$(target-dir)/loader.bin: loader.asm fat12header.inc
	$(ASM) $(ASMFLAGS) -o $@ $<
