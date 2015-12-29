ASM=nasm
NASMFLAGS=-O0 -f bin

QEMU=qemu-system-i386
QEMUFLAGS=-s -boot a

IMG=bootloader.img
BINNAME=boot0

all:  $(BINNAME).bin


$(BINNAME).bin: $(BINNAME).asm
	$(ASM) $(NASMFLAGS) -o $@ -l $(BINNAME).lst $< 

$(IMG): $(BINNAME).bin
	cat $< /dev/zero | dd of=$@ bs=512 count=2880

clean:
	-rm -f *.o *.lst *.bin $(IMG)



run-qemu: $(IMG)
	$(QEMU) $(QEMUFLAGS) -hda $<

run-qemu-with-gdb: $(IMG)
	$(QEMU) -S $(QEMUFLAGS) -hda $<

run-gdb:
	gdb -ex 'target remote localhost:1234' \
		-ex 'set architecture i8086' \
		-ex 'break *0x7c00' \
		-ex 'continue'

disassemble: $(BINNAME).bin
	objdump -D -b binary --stop-address=0x005c -mi386 -Maddr16,data16 $< | less
