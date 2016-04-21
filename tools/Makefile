CC=gcc
CFLAGS=-W -Werror -Wall -std=c11
LD=gcc

ELF_MBRPART=mbrpart

SRC=$(wildcard *.c);
OBJ=$(SRC:.c=.o)

$(ELF_MBRPART).elf: $(ELF_MBRPART).o

all: $(ELF_MBRPART)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.elf: %.o
	$(LD) -o $@ $^
	mv $@ $*

##############
## CLEAN UP ##
##############

clean:
	-rm -f *.o

mrproper: clean
	-rm -f $(ELF_MBRPART)
