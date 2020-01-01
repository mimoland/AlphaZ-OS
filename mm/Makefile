

# 当前Makefile相对于项目根目录的路径，子Makefile共用，注意被覆盖
reldir := mm
# 每当有新的目录加入时，需要修改该变量，子目录共用此变量，注意可能被覆盖
subdir := .
subdir := $(addprefix $(reldir)/, $(subdir))

src-mm += $(foreach dir, $(subdir), $(wildcard $(dir)/*.*))


obj-mm := $(patsubst %.c, %.o, $(src-mm))
obj-mm := $(patsubst %.asm, %.oa, $(obj-mm))
obj-mm := $(addprefix $(build)/, $(obj-mm))

$(build)/mm.a: $(obj-mm)
	$(AR) $(ARFLAGS) $@ $^
