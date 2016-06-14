PROJECT_ROOT:=.

# project config
include $(PROJECT_ROOT)/project.config

# sources
include $(BOOT0_DIR)/make.config
include $(ARCH_DIR)/make.config
include $(LIBC_DIR)/make.config
include $(SHELL_DIR)/make.config
include $(COMMON_DIR)/make.config


IMG=harddisk.img
STAGE0=boot0
STAGE1=bootloader
KMULTIBOOT=kmultiboot

OBJ_STAGE0=$(BOOT0_SRC_NASM:.asm=.o)
OBJ_STAGE0+=$(BOOT0_SRC_GAS:.S=.o)
#$(info $$OBJ_STAGE0 = [${OBJ_STAGE0}])

ifdef BOOTLOADER_MINIMAL
OBJ_STAGE1=$(COMMON_DIR)/main.o
OBJ_STAGE1+=$(COMMON_DIR)/sys/panic.o
ifdef FLAG_PROTECTED_MODE
OBJ_STAGE1+=$(ARCH_DIR)/lib/int32.o
else
OBJ_STAGE1+=$(ARCH_DIR)/lib/bioscall.o
endif
OBJ_STAGE1+=$(ARCH_DIR)/lib/system.o
OBJ_STAGE1+=$(ARCH_DIR)/lib/regs.o
OBJ_STAGE1+=$(ARCH_DIR)/lib/tty.o
OBJ_STAGE1+=$(ARCH_DIR)/lib/gdt.o
OBJ_STAGE1+=$(LIBC_DIR)/assert/assert.o
OBJ_STAGE1+=$(LIBC_DIR)/stdio/basic_out.o
OBJ_STAGE1+=$(LIBC_DIR)/stdio/printf.o
OBJ_STAGE1+=$(LIBC_DIR)/string/mem_operations.o
OBJ_STAGE1+=$(LIBC_DIR)/string/str_operations.o
OBJ_STAGE1+=$(LIBC_DIR)/string/nb_to_str.o
else
OBJ_STAGE1=$(ARCH_SRC_WITHOUT_BOOTSTRAP_NASM:.asm=.o)
OBJ_STAGE1+=$(ARCH_SRC_WITHOUT_BOOTSTRAP_GAS:.S=.o)
OBJ_STAGE1+=$(ARCH_SRC_WITHOUT_BOOTSTRAP_C:.c=.o)
OBJ_STAGE1+=$(LIBC_SRC:.c=.o)
OBJ_STAGE1+=$(SHELL_SRC:.c=.o)
OBJ_STAGE1+=$(COMMON_SRC:.c=.o)
endif

#$(info $$OBJ_STAGE1 = [${OBJ_STAGE1}])

ELF_STAGE0=$(STAGE0).elf
ELF_STAGE1=$(STAGE1).elf
ELF_KMULTIBOOT=$(KMULTIBOOT).elf
BIN_STAGE0=$(STAGE0).bin
BIN_STAGE1=$(STAGE1).bin

ELF= $(ELF_STAGE0) $(ELF_STAGE1)
BIN= $(BIN_STAGE0) $(BIN_STAGE1)
#BIN= $(ELF:.elf=.bin)

all: dir-tools $(ELF_KMULTIBOOT) $(BIN)

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
	$(CC) -T$(STAGE1_LD_SCRIPT) -o $@ $^ $(LDFLAGS)

$(ELF_STAGE0): $(OBJ_STAGE0)
	$(CC) -T$(STAGE0_LD_SCRIPT) -o $@ $^ $(LDFLAGS)

$(ELF_KMULTIBOOT):
	$(MAKE) -C $(KMULTIBOOT_DIR)

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

$(IMG): all
	sudo $(TOOLS_DIR)/partitioning.sh /dev/loop0 -o $@ $(BIN_STAGE0) $(BIN_STAGE1)
	sudo $(TOOLS_DIR)/formatting.sh /dev/loop0 $@
	sudo $(TOOLS_DIR)/fcopytofs.sh /dev/loop0 $@ $(KMULTIBOOT_ROOT_FS_DIR)
	sudo chown matt:matt $@


##############
## CLEAN UP ##
##############

clean: clean-arch clean-libc clean-shell clean-common clean-tools clean-kmultiboot

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

clean-kmultiboot:
	-$(MAKE) -C $(KMULTIBOOT_DIR) mrproper

mrproper: clean
	-find . -iname "*.img" -delete
	-find . -iname "*.bin" -delete
	-find . -iname "*.elf" -delete


# Inspection tools
disassemble-bin:
	$(OBJDUMP) -D -b binary $(ARCH_OBJDUMPFLAGS) $(IMG) | less

disassemble-elf-stage0: $(ELF_STAGE0)
	$(OBJDUMP) -D -mi386 -Maddr16,data16 $< | less

disassemble-elf-stage1: $(ELF_STAGE1)
	$(OBJDUMP) -D $(ARCH_OBJDUMPFLAGS) $< | less

hexdump:
	xxd -c 1 $(IMG) | less

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
