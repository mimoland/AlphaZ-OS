

# 当前Makefile相对于项目根目录的路径，子Makefile共用，注意被覆盖
reldir := init
# 每当有新的目录加入时，需要修改该变量，子目录共用此变量，注意可能被覆盖
subdir := .
subdir := $(addprefix $(reldir)/, $(subdir))

src-init += $(foreach dir, $(subdir), $(wildcard $(dir)/*.*))


obj-init := $(patsubst %.c, %.o, $(src-init))
obj-init := $(patsubst %.asm, %.oa, $(obj-init))
obj-init := $(addprefix $(build)/, $(obj-init))

$(build)/init.a: $(obj-init)
	$(AR) $(ARFLAGS) $@ $^
