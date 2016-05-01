PROJECT_ROOT:=.

# project config
include $(PROJECT_ROOT)/project.config

# sources
include $(BOOT0_DIR)/make.config
include $(ARCH_DIR)/make.config
include $(LIBC_DIR)/make.config
include $(SHELL_DIR)/make.config
include $(COMMON_DIR)/make.config


IMG=bootloader.img

ELF_STAGE0=boot0.elf
ELF_STAGE1=bootloader.elf

OBJ_STAGE0=$(BOOT0_SRC_NASM:.asm=.o)
OBJ_STAGE0+=$(BOOT0_SRC_GAS:.S=.o)
#$(info $$OBJ_STAGE0 = [${OBJ_STAGE0}])

OBJ_STAGE1=$(ARCH_SRC_WITHOUT_BOOTSTRAP_NASM:.asm=.o)
OBJ_STAGE1+=$(ARCH_SRC_WITHOUT_BOOTSTRAP_GAS:.S=.o)
OBJ_STAGE1+=$(ARCH_SRC_WITHOUT_BOOTSTRAP_C:.c=.o)
OBJ_STAGE1+=$(LIBC_SRC:.c=.o)
OBJ_STAGE1+=$(SHELL_SRC:.c=.o)
OBJ_STAGE1+=$(COMMON_SRC:.c=.o)

#$(info $$OBJ_STAGE1 = [${OBJ_STAGE1}])

ELF= $(ELF_STAGE0) $(ELF_STAGE1)
BIN= $(ELF:.elf=.bin)

all: dir-tools $(IMG)

dir-arch:
	$(MAKE) -C $(ARCH_DIR)

dir-shell:
	$(MAKE) -C $(SHELL_DIR)

dir-libc:
	$(MAKE) -C $(LIBC_DIR)

dir-common:
	$(MAKE) -C $(COMMON_DIR)

dir-tools:
	$(MAKE) -C $(TOOLS_DIR)

$(ELF_STAGE1): $(OBJ_STAGE1)
	$(MAKE) -C $(ARCH_DIR) bootstrap.o
	$(MAKE) -C $(ARCH_DIR) linker.ld
	$(LD) $(LDFLAGS) -T$(STAGE1_LD_SCRIPT) -o $@ $^

$(ELF_STAGE0): $(OBJ_STAGE0)
	$(LD) $(LDFLAGS) -T$(STAGE0_LD_SCRIPT) -o $@ $^

%.o: %.asm
	$(NASM) $(NASMFLAGS) -o $@ -l $*.lst $<

%.o: %.S
	$(CC) $(ASFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

#%.elf: %.o
#	$(LD) $(LDFLAGS) -T$(LD_SCRIPT) -o $@ $^

%.bin: %.elf
	$(OBJCOPY) --remove-section=.comment --remove-section=.note -O binary $< $@
	@echo `du -h $@`

$(IMG): $(BIN)
	$(TOOLS_DIR)/mbrpart boot0.bin
	cat $^ /dev/zero | dd of=$@ bs=512 count=2880


##############
## CLEAN UP ##
##############

clean: clean-arch clean-libc clean-shell clean-common clean-tools

clean-arch:
	-$(MAKE) -C $(ARCH_DIR) clean

clean-libc:
	-$(MAKE) -C $(LIBC_DIR) clean

clean-shell:
	-$(MAKE) -C $(SHELL_DIR) clean

clean-common:
	-$(MAKE) -C $(COMMON_DIR) clean

clean-tools:
	-$(MAKE) -C $(TOOLS_DIR) mrproper

mrproper: clean
	-find . -iname "*.img" -delete
	-find . -iname "*.bin" -delete
	-find . -iname "*.elf" -delete


# Inspection tools
disassemble-bin: $(BINARY)
	$(OBJDUMP) -D -b binary $(ARCH_OBJDUMPFLAGS) $< | less

disassemble-elf-stage0: $(ELF_STAGE0)
	$(OBJDUMP) -D -mi386 -Maddr16,data16 $< | less

disassemble-elf-stage1: $(ELF_STAGE1)
	$(OBJDUMP) -D $(ARCH_OBJDUMPFLAGS) $< | less

hexdump: $(BINARY)
	xxd -c 1 $< | less

# Simulation
run-qemu: $(IMG)
	$(QEMU) $(QEMUFLAGS) -hda $<

# Debug tools
run-qemu-with-gdb: $(IMG)
	$(QEMU) -S $(QEMUFLAGS) -hda $<

run-gdb:
	gdb -ex 'target remote localhost:1234' \
		-ex 'set architecture i386' \
		-ex 'break *0x7c00' \
		-ex 'continue'

