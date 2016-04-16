PROJECT_ROOT:=..

#project config
include $(PROJECT_ROOT)/project.config

# sources
include make.config 

all: $(LIBC_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	-find . -name "*.o" -delete
	-find . -name "*.lst" -delete
