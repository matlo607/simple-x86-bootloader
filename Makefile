ASM=nasm
NASMFLAGS=-O0 -f bin

QEMU=qemu-system-i386
QEMUFLAGS=-s -boot a

SRC_ASM= $(wildcard *.asm)
BIN= $(SRC_ASM:.asm=.bin)

IMG=bootloader.img

all:  $(BIN)


%.bin: %.asm
	$(ASM) $(NASMFLAGS) -o $@ -l $*.lst $<

$(IMG): $(BIN)
	cat $^ /dev/zero | dd of=$@ bs=512 count=2880

clean:
	-rm -f *.o *.lst *.bin $(IMG)



# Inspection tools
disassemble: $(BINARY)
	objdump -D -b binary -mi386 -Maddr16,data16 $< | less

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

