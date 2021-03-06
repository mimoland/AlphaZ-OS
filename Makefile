# 该Makefile是项目的顶层Makefile，管理整个项目的编译工作

MAKEFLAGS  += -rR --no-print-directory

DEBUG = 1

AS 			= as
DASM		= ndisasm
CC			= gcc
LD			= ld
AR 			= ar
ASFLAGS 	=
CFLAGS		= -I include/ -c -O2 -Wall -fno-builtin -fno-common \
				-fno-stack-protector
LDFLAGS		= -T scripts/kernel.lds --whole-archive
ARFLAGS		= -rc
DASMFLAGS	=
PHONY		:= _all

export AS DASM CC LD AR
export ASFLAGS CFLAGS LDFLAGS ARFLAGS DASMFLAGS
export PHONY

# 欲生成平台的处理器架构
ARCH		?= i386
export ARCH

ifeq ($(ARCH), i386)
	ASFLAGS += --32
	CFLAGS += -m32
	LDFLAGS += -m elf_i386
endif

ifeq ($(DEBUG), 1)
	CFLAGS += -g
endif

srctree	:= $(CURDIR)
build	:= build
target	:= target
src-all	:=
libs	:= arch.a init.a kernel.a mm.a drivers.a fs.a lib.a
libs 	:= $(addprefix $(build)/, $(libs))

_all: all


$(target)/kernel.bin: $(libs)
	$(LD) $(LDFLAGS) -o $@ $^

include arch/$(ARCH)/Makefile
src-all += $(src-arch)

include drivers/Makefile
src-all += $(src-drivers)

include fs/Makefile
src-all += $(src-fs)

include init/Makefile
src-all += $(src-init)

include lib/Makefile
src-all += $(src-lib)

include mm/Makefile
src-all += $(src-mm)

include kernel/Makefile
src-all += $(src-kernel)

all: config $(target)/kernel.bin
	$(MAKE) -C arch/$(ARCH)/boot
PHONY += all

buildimg:
	dd if=$(target)/mbr.bin of=alphaz.vhd bs=512 count=1 conv=notrunc
	dd if=$(target)/boot.bin of=alphaz.vhd bs=512 count=1 seek=2048 conv=notrunc
	sudo modprobe nbd max_part=8
	sudo qemu-nbd -f vpc -c /dev/nbd0 alphaz.vhd
	sleep 0.5
	sudo mount /dev/nbd0p1 /mnt/
	sudo rm /mnt/*
	sudo cp -fv $(target)/loader.bin /mnt/
	sudo cp -fv $(target)/kernel.bin /mnt/
	sudo umount /mnt
	sudo qemu-nbd -d /dev/nbd0
PHONY += buildimg

image: all buildimg
PHONY += image

# 编译前对整个项目的配置。包括相关目录的生成以及创建目录链接等工作。所有子目录的Makefile的导入
# 工作必须在此前完成。

config: $(dir $(src-all))
	@ln -fsn $(srctree)/include/asm-$(ARCH) include/asm
	@test -d $(target) || mkdir -p $(target)
	@for i in $^; \
	do \
		test -d $(build)/$$i || mkdir -p $(build)/$$i; \
	done

PHONY += config


clean:
	rm -rf $(build)
	rm -rf $(target)
PHONY += clean

qemu:
	qemu -hda alphaz.vhd -m 2G,slots=3,maxmem=4G
PHONY += qemu

debug:
	@qemu -s -S -hda alphaz.vhd -m 2G,slots=3,maxmem=4G &
	@gdb -x scripts/gdbinit
	@kill $$(ps | grep qemu | awk '{print $$1 }')


PHONY += debug

# 下面定义一些通用的规则，为每个.o文件生成相应的源文件和头文件依赖。生成前必须要先对项目进行基
# 本的配置，保证文件创建的路径正确，所以依赖config。然后将生成的依赖进行导入
$(build)/%.d: %.c config
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed '1s,^,$@ $(dir $@),' $@.$$$$  | \
	sed '$$a \\t$(CC) $(CFLAGS) -o $(patsubst %.d,%.o,$@) $<' >  $@; \
	rm -f $@.$$$$

-include $(addprefix $(build)/, $(patsubst %.c, %.d, $(filter %.c, $(src-all))))

$(build)/%.o: %.S
	$(CC) $(CFLAGS) -E -D__ASSEMBLY__ $< > $(addprefix $(build)/, $(patsubst %.S, %.s, $<))
	$(AS) $(ASFLAGS) -o $@ $(addprefix $(build)/, $(patsubst %.S, %.s, $<))


.PHONY = $(PHONY)
