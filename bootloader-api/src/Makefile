ARCH_PREFIX=i386-elf

INCDIR=../inc

AS=$(ARCH_PREFIX)-as
ASFLAGS=-c -march=i386 -fno-asynchronous-unwind-tables -W -I$(INCDIR)

NASM=nasm
NASMFLAGS=-O0 -f elf32

LD=$(ARCH_PREFIX)-ld
LDFLAGS=-m elf_i386 -static -nostdlib --nmagic

CC=$(ARCH_PREFIX)-gcc
CFLAGS=-std=c99 -O0 -c -march=i386 -m16 -ffreestanding -fno-asynchronous-unwind-tables -Wall -W -I$(INCDIR)

OBJCOPY=$(ARCH_PREFIX)-objcopy

SRC_NASM= $(wildcard *.asm)
SRC_GAS= $(wildcard *.S)
SRC_C= $(wildcard *.c)
OBJ= $(SRC_ASM:.asm=.o)
OBJ+= $(SRC_GAS:.S=.o)
OBJ+= $(SRC_C:.c=.o)

all: $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(NASM) $(NASMFLAGS) -o $@ -l $*.lst $<

%.o: %.S
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	-rm -f *.o *.lst

