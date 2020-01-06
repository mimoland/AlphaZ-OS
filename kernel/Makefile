

# 当前Makefile相对于项目根目录的路径，子Makefile共用，注意被覆盖
reldir := kernel
# 每当有新的目录加入时，需要修改该变量，子目录共用此变量，注意可能被覆盖
subdir := .
subdir := $(addprefix $(reldir)/, $(subdir))

src-kernel += $(foreach dir, $(subdir), $(wildcard $(dir)/*.*))


obj-kernel := $(patsubst %.c, %.o, $(src-kernel))
obj-kernel := $(patsubst %.asm, %.oa, $(obj-kernel))
obj-kernel := $(addprefix $(build)/, $(obj-kernel))

$(build)/kernel.a: $(obj-kernel)
	$(AR) $(ARFLAGS) $@ $^
