PROJECT_ROOT:=..

MULTIBOOT_KERNEL:= -DMULTIBOOT_KERNEL

include $(PROJECT_ROOT)/project.config
#include $(PROJECT_ROOT)/arch/x86/arch.config
#include $(PROJECT_ROOT)/make.config


$(info $$ASFLAGS = [${ASFLAGS}])
$(info $$CFLAGS = [${CFLAGS}])

# sources
include make.config 

KERNEL_MULTIBOOT_COMPLIANT=kmultiboot
ELF_KMULTIBOOT_COMPLIANT=$(KERNEL_MULTIBOOT_COMPLIANT).elf

all: $(KMULTIBOOT_OBJS) $(ELF_KMULTIBOOT_COMPLIANT)

$(ELF_KMULTIBOOT_COMPLIANT): $(KMULTIBOOT_OBJS_LINKER)
	$(CC) -T$(KMULTIBOOT_COMPLIANT_LD_SCRIPT) -o $@ $^ $(LDFLAGS)
	-mkdir -p $(KMULTIBOOT_ROOT_FS_DIR)
	-cp $@ $(KMULTIBOOT_ROOT_FS_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.S
	$(CC) $(ASFLAGS) -o $@ $<

run-qemu: $(KMULTIBOOT_OBJS) $(ELF_KMULTIBOOT_COMPLIANT)
	$(QEMU) -kernel $(ELF_KMULTIBOOT_COMPLIANT)

clean:
	-rm -f *.o

mrproper: clean
	-rm -f $(ELF_KMULTIBOOT_COMPLIANT)
	-rm -rf $(KMULTIBOOT_ROOT_FS_DIR)
