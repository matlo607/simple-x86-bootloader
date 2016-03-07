ARCH_PREFIX=i386-elf

INCDIR=../inc

ASM=nasm
NASMFLAGS=-O0 -f elf32

LD=$(ARCH_PREFIX)-ld
LDFLAGS=-m elf_i386 -static -nostdlib --nmagic

CC=$(ARCH_PREFIX)-gcc
CFLAGS=-std=c99 -DDEBUG -O0 -c -march=i386 -m16 -ffreestanding -fno-asynchronous-unwind-tables -Wall -W -I$(INCDIR)

OBJCOPY=$(ARCH_PREFIX)-objcopy

SRC_ASM= $(wildcard *.asm)
SRC_C= $(wildcard *.c)
OBJ= $(SRC_ASM:.asm=.o)
OBJ+= $(SRC_C:.c=.o)

all: $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(ASM) $(NASMFLAGS) -o $@ -l $*.lst $<

clean:
	-rm -f *.o

