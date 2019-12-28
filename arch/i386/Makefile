# 该Makefile被顶层Makefile包含，用于编译arch目录下源文件。以下规则中所有的源文件的路径都是
# 根据项目根目录而言的，这样简化了顶层Makefile的规则，方便顶层Makefile构建。
# 每当arch子目录中有新的源文件加入，都需要将其相对于该Makefile的路径添加到src-arch中，顶层
# Makefile将会使用此变量。
# boot和loader的构建比较特殊，不使用通用规则，而是单独进行编译到target目录，若boot和loader
# 不改变无需改变其规则
# arch目录中平台相关的代码将会编译成静态库的形式供顶层Makefile进行链接，arch目录中的源文件最
# 终会编译成arch.a静态库，编译成静态库的规则在该Makefile文件最后给出


ASMBFLAGS += -I arch/i386/include/

subarch := arch/$(ARCH)


# 每当有新文件的加入，都要都需要将其相对于该Makefile的路径加入到该变量，该变量会在顶层
# Makefile中被引用
src-arch += kernel/start.c kernel/start.asm	kernel/i8259.c kernel/int.c \
		kernel/global.c lib/bug.c

src-arch := $(addprefix $(subarch)/, $(src-arch))


# boot.bin和loader.bin比较简单，不使用通用规则，单独编译到target文件夹。boot变量会在顶层
# Makefile中被引用
boot = $(target)/boot.bin $(target)/loader.bin

$(target)/boot.bin: $(subarch)/boot/boot.asm $(subarch)/include/fat12header.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(target)/loader.bin: $(subarch)/loader/loader.asm \
						$(subarch)/include/fat12header.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<


# 将arch目录下处boot和loader的所有文件编译成arch.a静态库到$(build)目录中
obj-arch := $(patsubst %.c, %.o, $(src-arch))
obj-arch := $(patsubst %.asm, %.oa, $(obj-arch))
obj-arch := $(addprefix $(build)/, $(obj-arch))

$(build)/arch.a: $(obj-arch)
	$(AR) $(ARFLAGS) $@ $^
