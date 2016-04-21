PROJECT_ROOT:=../../..

include ../arch.config
include $(PROJECT_ROOT)/make.config
include make.config

LD_SCRIPT=boot0.ld

ELF_STAGE0=boot0.elf
BIN=$(ELF_STAGE0:.elf=.bin)
IMG=bootloader.img

all: $(BIN)

$(ELF_STAGE0): $(BOOT0_OBJ)

%.o: %.asm
	$(NASM) $(NASMFLAGS) -o $@ -l $*.lst $<

%.o: %.S
	$(CC) $(ASFLAGS) -o $@ $<

%.elf: %.o
	$(LD) $(LDFLAGS) -T$(LD_SCRIPT) -o $@ $^

%.bin: %.elf
	$(OBJCOPY) --remove-section=.comment --remove-section=.note -O binary $< $@

$(IMG): $(BIN)
	cat $^ /dev/zero | dd of=$@ bs=512 count=2880

clean:
	-rm -f *.o *.lst

mrproper: clean
	-rm -f *.elf *.bin $(IMG)

# Inspection tools
disassemble-bin: $(BIN)
	$(OBJDUMP) -D -b binary -mi386 -Maddr16,data16 $< | less

disassemble-elf-stage0: $(ELF_STAGE0)
	$(OBJDUMP) -D -mi386 -Maddr16,data16 $< | less

hexdump: $(BIN)
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

