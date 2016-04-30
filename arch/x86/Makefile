PROJECT_ROOT:=../..

#project config
include $(PROJECT_ROOT)/project.config

# sources
include make.config 

all: $(ARCH_OBJS) linker.ld

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(NASM) $(NASMFLAGS) -o $@ -l $*.lst $<

%.o: %.S
	$(CC) $(ASFLAGS) -o $@ $<

linker.ld:
	$(CC) $(PREPROCESSORFLAGS) -I$(PROJECT_ROOT)/include -P -E $(STAGE1_LD_SCRIPT).h -o $(STAGE1_LD_SCRIPT)

clean:
	-find . -name "*.o" -delete
	-find . -name "*.lst" -delete
	-rm -f *.ld
