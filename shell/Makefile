ARCH_PREFIX=i386-elf

#################
## Directories ##
#################

BOOTLOADER_API_DIR=../bootloader-api
BOOTLOADER_API_INC_PATH=$(BOOTLOADER_API_DIR)/inc

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
CFLAGS=-std=c99 -O0 -c -march=i386 -m16 -ffreestanding -fno-asynchronous-unwind-tables -W -Wall -I$(BOOTLOADER_API_INC_PATH)

##################
## DEPENDENCIES ##
##################

SRC_NASM= $(wildcard *.asm)
SRC_GAS= $(wildcard *.S)
SRC_C= $(wildcard *.c)

OBJ= $(SRC_NASM:.asm=.o)
OBJ+= $(SRC_GAS:.S=.o)
OBJ+= $(SRC_C:.c=.o)

all: $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(ASM) $(NASMFLAGS) -o $@ -l $*.lst $<

%.o: %.S
	$(AS) $(ASFLAGS) -o $@ $<

##############
## CLEAN UP ##
##############

clean:
	-rm -f *.o *.lst
