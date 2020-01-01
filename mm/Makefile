submm := mm


# 每当有新文件的加入，都要都需要将其相对于该Makefile的路径加入到该变量，该变量会在顶层
# Makefile中被引用
src-mm += mm_init.c malloc.c

src-mm := $(addprefix $(submm)/, $(src-mm))


obj-mm := $(patsubst %.c, %.o, $(src-mm))
obj-mm := $(patsubst %.asm, %.oa, $(obj-mm))
obj-mm := $(addprefix $(build)/, $(obj-mm))

$(build)/mm.a: $(obj-mm)
	$(AR) $(ARFLAGS) $@ $^
