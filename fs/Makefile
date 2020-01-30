
# 当前Makefile相对于项目根目录的路径，子Makefile共用，注意被覆盖
reldir := fs
# 每当有新的目录加入时，需要修改该变量，子目录共用此变量，注意可能被覆盖
subdir := .
subdir := $(addprefix $(reldir)/, $(subdir))

src-fs := $(foreach dir, $(subdir), $(wildcard $(dir)/*.*))


obj-fs := $(patsubst %.c, %.o, $(src-fs))
obj-fs := $(addprefix $(build)/, $(obj-fs))

$(build)/fs.a: $(obj-fs)
	$(AR) $(ARFLAGS) $@ $^
