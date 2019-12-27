subinit := init


# 每当有新文件的加入，都要都需要将其相对于该Makefile的路径加入到该变量，该变量会在顶层
# Makefile中被引用
src-init += main.c

src-init := $(addprefix $(subinit)/, $(src-init))


obj-init := $(patsubst %.c, %.o, $(src-init))
obj-init := $(patsubst %.asm, %.oa, $(obj-init))
obj-init := $(addprefix $(build)/, $(obj-init))

$(build)/init.a: $(obj-init)
	$(AR) $(ARFLAGS) $@ $^
