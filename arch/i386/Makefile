
# 当前Makefile相对于项目根目录的路径，子Makefile共用，注意被覆盖
reldir := arch/$(ARCH)
# 每当有新的目录加入时，需要修改该变量，子目录共用此变量，注意可能被覆盖
subdir := lib kernel drivers boot
subdir := $(addprefix $(reldir)/, $(subdir))

src-arch += $(foreach dir, $(subdir), $(wildcard $(dir)/*.*))

obj-arch := $(patsubst %.c, %.o, $(src-arch))
obj-arch := $(patsubst %.S, %.o, $(obj-arch))
obj-arch := $(addprefix $(build)/, $(obj-arch))

$(build)/arch.a: $(obj-arch)
	$(AR) $(ARFLAGS) $@ $^
