asm_kernel_init_dir		:= init
asm_kernel_sys_dir		:= sys
modules		:= $(asm_kernel_init_dir) $(asm_kernel_sys_dir)

.PHONY:	build clean $(modules)

all: build

build: $(modules)

$(modules):
	$(MAKE) all --directory=$@

clean:
	for d in $(modules); \
		do \
			$(MAKE) --directory=$$d clean; \
		done
	rm -rf *.o *~

include ../../../../Makefile.in
