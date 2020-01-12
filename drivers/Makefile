

# 当前Makefile相对于项目根目录的路径，子Makefile共用，注意被覆盖
reldir := drivers
# 每当有新的目录加入时，需要修改该变量，子目录共用此变量，注意可能被覆盖
subdir := .
subdir := $(addprefix $(reldir)/, $(subdir))

src-drivers += $(foreach dir, $(subdir), $(wildcard $(dir)/*.*))


obj-drivers := $(patsubst %.c, %.o, $(src-drivers))
obj-drivers := $(patsubst %.asm, %.oa, $(obj-drivers))
obj-drivers := $(addprefix $(build)/, $(obj-drivers))

$(build)/drivers.a: $(obj-drivers)
	$(AR) $(ARFLAGS) $@ $^
