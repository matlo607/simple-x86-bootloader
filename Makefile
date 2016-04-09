ARCH_PREFIX=i386-elf

#################
## Directories ##
#################

BOOTLOADER_API_DIR=./bootloader-api

BOOTLOADER_API_SRC_PATH=$(BOOTLOADER_API_DIR)/src
BOOTLOADER_API_INC_PATH=$(BOOTLOADER_API_DIR)/inc
BOOTLOADER_API_OBJ_PATH=$(BOOTLOADER_API_DIR)/src

BOOT0_DIR=./boot0

#####################
## TOOLS AND FLAGS ##
#####################

ASM=nasm
NASMFLAGS=-O0 -f elf32

LD=$(ARCH_PREFIX)-ld
LDFLAGS=-m elf_i386 -static -nostdlib --nmagic

CC=$(ARCH_PREFIX)-gcc
CFLAGS=-std=c99 -O0 -c -march=i386 -m16 -ffreestanding -fno-asynchronous-unwind-tables -W -Wall -I$(BOOTLOADER_API_INC_PATH)

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


SRC_ASM= $(wildcard *.asm)
SRC_C= $(wildcard *.c)
SRC_API_C= $(wildcard $(BOOTLOADER_API_SRC_PATH)/*.c)

OBJ= $(SRC_ASM:.asm=.o)
OBJ+= $(SRC_C:.c=.o)
OBJ_API= $(SRC_API_C:.c=.o)

ELF= $(ELF_STAGE0) $(ELF_STAGE1)
BIN= $(ELF:.elf=.bin)


all: $(BIN)


$(OBJ_API):
	$(MAKE) -C $(BOOTLOADER_API_SRC_PATH)

$(ELF_STAGE0):
	$(MAKE) -C $(BOOT0_DIR)

$(ELF_STAGE1): $(OBJ_API) bootstrap.o bootloader.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(ASM) $(NASMFLAGS) -o $@ -l $*.lst $<

%.elf: %.o
	$(LD) $(LDFLAGS) -T$*.ld -o $@ $^

%.bin: %.elf
	$(OBJCOPY) --remove-section=.comment -O binary $< $@

$(IMG): $(BIN)
	cat $^ /dev/zero | dd of=$@ bs=512 count=2880


##############
## CLEAN UP ##
##############

clean:
	-rm -f *.o *.elf *.lst *.bin $(IMG)

clean-api:
	-$(MAKE) -C $(BOOTLOADER_API_SRC_PATH) clean

clean-boot0:
	-$(MAKE) -C $(BOOT0_DIR) clean

clean-all: clean-boot0 clean-api clean


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

