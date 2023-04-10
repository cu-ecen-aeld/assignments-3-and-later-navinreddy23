SRC := aesdsocket.c str_ops.c
TARGET = aesdsocket
OBJS := $(SRC:.c=.o)

CROSS_COMPILE=
CC=${CROSS_COMPILE}gcc
CFLAGS= -g -Wall -Werror
all: $(TARGET)

$(TARGET) : $(OBJS)
	echo "Compiler: $(CC)"
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	-rm -f *.o $(TARGET) *.elf *.map