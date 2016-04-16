PROJECT_ROOT:=../..

#project config
include $(PROJECT_ROOT)/project.config

# sources
include make.config 

all: $(ARCH_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(NASM) $(NASMFLAGS) -o $@ -l $*.lst $<

%.o: %.S
	$(CC) $(ASFLAGS) -o $@ $<

clean:
	-find . -name "*.o" -delete
	-find . -name "*.lst" -delete
