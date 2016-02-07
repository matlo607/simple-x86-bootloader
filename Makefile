ARCH_PREFIX=i386-elf

BOOTLOADER_API_DIR=./bootloader-api

BOOTLOADER_API_SRC_PATH=$(BOOTLOADER_API_DIR)/src
BOOTLOADER_API_INC_PATH=$(BOOTLOADER_API_DIR)/inc
BOOTLOADER_API_OBJ_PATH=$(BOOTLOADER_API_DIR)/src

ASM=nasm
NASMFLAGS=-O0 -f elf32

LD=$(ARCH_PREFIX)-ld
LDFLAGS=-m elf_i386 -static -nostdlib --nmagic

CC=$(ARCH_PREFIX)-gcc
CFLAGS=-std=c99 -DDEBUG -O0 -c -g -march=i386 -m16 -ffreestanding -W -Wall -I$(BOOTLOADER_API_INC_PATH)

OBJDUMP=$(ARCH_PREFIX)-objdump
OBJCOPY=$(ARCH_PREFIX)-objcopy

QEMU=qemu-system-i386
QEMUFLAGS=-s -cpu pentium3 -boot a -display sdl -vga std -k fr -m 64M

SRC_ASM= $(wildcard *.asm)
SRC_C= $(wildcard *.c)
OBJ= $(SRC_ASM:.asm=.o)
OBJ+= $(SRC_C:.c=.o)
ELF= boot0.elf bootloader.elf
BIN= $(ELF:.elf=.bin)

IMG=bootloader.img

all: $(BIN)

boot0.elf: boot0.o
bootloader.elf: bootstrap.o bootloader.o $(BOOTLOADER_API_OBJ_PATH)/*.o

BINARY=bootloader.img
ELF_STAGE0=boot0.elf
ELF_STAGE1=bootloader.elf

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(ASM) $(NASMFLAGS) -o $@ -l $*.lst $<

%.elf: %.o
	$(LD) $(LDFLAGS) -T$*.ld -o $@ $^

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

$(IMG): $(BIN)
	cat $^ /dev/zero | dd of=$@ bs=512 count=2880

clean:
	-rm -f *.o *.elf *.lst *.bin $(IMG)


bootloader-api:
	$(MAKE) -C $(BOOTLOADER_API_SRC_PATH)



# Inspection tools
disassemble-bin: $(BINARY)
	$(OBJDUMP) -D -b binary -mi386 -Maddr16,data16 $< | less

disassemble-elf-stage0: $(ELF_STAGE0)
	$(OBJDUMP) -d -mi386 -Maddr16,data16 $< | less

disassemble-elf-stage1: $(ELF_STAGE1)
	$(OBJDUMP) -d -mi386 -Maddr16,data16 $< | less

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

