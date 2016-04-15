ARCH_PREFIX=i386-elf

#################
## Directories ##
#################

BOOTLOADER_API_DIR=./bootloader-api

BOOTLOADER_API_SRC_PATH=$(BOOTLOADER_API_DIR)/src
BOOTLOADER_API_INC_PATH=$(BOOTLOADER_API_DIR)/inc
BOOTLOADER_API_OBJ_PATH=$(BOOTLOADER_API_DIR)/src

BOOTLOADER_SHELL_DIR=./shell
BOOTLOADER_SHELL_SRC_PATH=$(BOOTLOADER_SHELL_DIR)
BOOTLOADER_SHELL_INC_PATH=$(BOOTLOADER_SHELL_DIR)
BOOTLOADER_SHELL_OBJ_PATH=$(BOOTLOADER_SHELL_DIR)

BOOT0_DIR=./boot0

#####################
## TOOLS AND FLAGS ##
#####################

AS=$(ARCH_PREFIX)-gcc
ASFLAGS=-c -march=i386 -ffreestanding -fno-asynchronous-unwind-tables -W -I$(BOOTLOADER_API_INC_PATH)

NASM=nasm
NASMFLAGS=-O0 -f elf32

LD=$(ARCH_PREFIX)-ld
LDFLAGS=-m elf_i386 -static -nostdlib --nmagic

CC=$(ARCH_PREFIX)-gcc
CFLAGS=-std=c99 -O0 -c -march=i386 -m16 -ffreestanding -fno-asynchronous-unwind-tables -W -Wall -I$(BOOTLOADER_API_INC_PATH) -I$(BOOTLOADER_SHELL_INC_PATH)

OBJDUMP=$(ARCH_PREFIX)-objdump
OBJCOPY=$(ARCH_PREFIX)-objcopy

QEMU=qemu-system-i386
QEMUFLAGS=-s -cpu pentium3 -boot a -display sdl -vga std -k fr -m 64M

##################
## DEPENDENCIES ##
##################

IMG=bootloader.img
BINARY=bootloader.img
ELF_STAGE0=$(BOOT0_DIR)/boot0.elf
ELF_STAGE1=bootloader.elf


SRC_NASM= $(wildcard *.asm)
SRC_GAS= $(wildcard *.S)
SRC_C= $(wildcard *.c)

SRC_API_NASM= $(wildcard $(BOOTLOADER_API_SRC_PATH)/*.asm)
SRC_API_GAS= $(wildcard $(BOOTLOADER_API_SRC_PATH)/*.S)
SRC_API_C= $(wildcard $(BOOTLOADER_API_SRC_PATH)/*.c)

SRC_SHELL_C= $(wildcard $(BOOTLOADER_SHELL_SRC_PATH)/*.c)

OBJ= $(SRC_NASM:.asm=.o)
OBJ+= $(SRC_GAS:.S=.o)
OBJ+= $(SRC_C:.c=.o)

OBJ_API= $(SRC_API_NASM:.asm=.o)
OBJ_API+= $(SRC_API_GAS:.S=.o)
OBJ_API+= $(SRC_API_C:.c=.o)

OBJ_SHELL= $(SRC_SHELL_C:.c=.o)

ELF= $(ELF_STAGE0) $(ELF_STAGE1)
BIN= $(ELF:.elf=.bin)


all: $(BIN)

$(OBJ_SHELL):
	$(MAKE) -C $(BOOTLOADER_SHELL_SRC_PATH)

$(OBJ_API):
	$(MAKE) -C $(BOOTLOADER_API_SRC_PATH)

$(ELF_STAGE0):
	$(MAKE) -C $(BOOT0_DIR)

$(ELF_STAGE1): $(OBJ_API) $(OBJ_SHELL) bootstrap.o bootloader.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(ASM) $(NASMFLAGS) -o $@ -l $*.lst $<

%.o: %.S
	$(AS) $(ASFLAGS) -o $@ $<

%.elf: %.o
	$(LD) $(LDFLAGS) -T$*.ld -o $@ $^

%.bin: %.elf
	$(OBJCOPY) --remove-section=.comment --remove-section=.note -O binary $< $@

$(IMG): $(BIN)
	cat $^ /dev/zero | dd of=$@ bs=512 count=2880


##############
## CLEAN UP ##
##############

clean:
	-rm -f *.o *.elf *.lst *.bin $(IMG)

clean-api:
	-$(MAKE) -C $(BOOTLOADER_API_SRC_PATH) clean

clean-shell:
	-$(MAKE) -C $(BOOTLOADER_SHELL_SRC_PATH) clean

clean-boot0:
	-$(MAKE) -C $(BOOT0_DIR) clean

clean-all: clean-boot0 clean-api clean-shell clean


# Inspection tools
disassemble-bin: $(BINARY)
	$(OBJDUMP) -D -b binary -mi386 -Maddr16,data16 $< | less

disassemble-elf-stage0: $(ELF_STAGE0)
	$(OBJDUMP) -D -mi386 -Maddr16,data16 $< | less

disassemble-elf-stage1: $(ELF_STAGE1)
	$(OBJDUMP) -D -mi386 -Maddr16,data16 $< | less

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
		-ex 'set architecture i8086' \
		-ex 'break *0x7c00' \
		-ex 'continue'

