# 当前Makefile相对于项目根目录的路径，子Makefile共用，注意被覆盖
reldir := lib
# 每当有新的目录加入时，需要修改该变量，子目录共用此变量，注意可能被覆盖
subdir := .
subdir := $(addprefix $(reldir)/, $(subdir))

src-lib += $(foreach dir, $(subdir), $(wildcard $(dir)/*.*))


obj-lib := $(patsubst %.c, %.o, $(src-lib))
obj-lib := $(addprefix $(build)/, $(obj-lib))

$(build)/lib.a: $(obj-lib)
	$(AR) $(ARFLAGS) $@ $^
