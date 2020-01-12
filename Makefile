# 该Makefile是项目的顶层Makefile，管理整个项目的编译工作

# kernel入口地址
ENTRYPOINT = 0x30400

MAKEFLAGS  += -rR --no-print-directory

DEBUG = 1

ASM			= nasm
DASM		= ndisasm
CC			= gcc
LD			= ld
AR 			= ar
ASMKFLAGS	= -f elf
CFLAGS		= -I include/ -c -O2 -Wall -fno-builtin -fno-common \
				-fno-stack-protector
LDFLAGS		= -s -Ttext $(ENTRYPOINT)
ARFLAGS		= -rc
DASMFLAGS	=
PHONY		:= _all

export ASM DASM CC LD AR
export ASMBFLAGS ASMKFLAGS CFLAGS LDFLAGS ARFLAGS DASMFLAGS
export PHONY

# 欲生成平台的处理器架构
ARCH		?= i386
export ARCH

ifeq ($(ARCH), i386)
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
libs	:= arch.a init.a kernel.a mm.a drivers.a
libs 	:= $(addprefix $(build)/, $(libs))

_all: all


$(target)/kernel.bin: $(libs)
	$(LD) $(LDFLAGS) -o $@ $^ $^

include arch/$(ARCH)/Makefile
src-all += $(src-arch)

include drivers/Makefile
src-all += $(src-drivers)

include init/Makefile
src-all += $(src-init)

include mm/Makefile
src-all += $(src-mm)

include kernel/Makefile
src-all += $(src-kernel)

all: config $(target)/kernel.bin $(boot)
PHONY += all

buildimg:
	dd if=$(target)/boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy/
	sudo cp -fv $(target)/loader.bin /mnt/floppy/
	sudo cp -fv $(target)/kernel.bin /mnt/floppy/
	sudo umount /mnt/floppy
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


debug:
	@echo $(src-all)
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


# 编译.asm文件的通用规则，由于.asm文件一般不使用头文件，所以编译方式比较简单。为了防止.asm编
# 译生成的目标文件与.c的目标文件冲突，所以使用.oa后缀名
$(build)/%.oa: %.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<


.PHONY = $(PHONY)
