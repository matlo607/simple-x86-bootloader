ASM=nasm
NASMFLAGS=-O0 -f bin

BINNAME=boot0

all:  $(BINNAME).bin

$(BINNAME).bin: $(BINNAME).asm
	$(ASM) $(NASMFLAGS) -o $@ -l $(BINNAME).lst $< 

clean:
	-rm -f *.o *.lst *.bin floppyBootloader

floppyBootloader: $(BINNAME).bin
	cat $< /dev/zero | dd of=$@ bs=512 count=2880

run: floppyBootloader
	qemu-system-i386 -s -boot a -fda $<

disassemble: $(BINNAME).bin
	objdump -D -b binary --stop-address=0x005c -mi386 -Maddr16,data16 $< | less
