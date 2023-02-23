SRC := writer.c
TARGET = writer
OBJS := $(SRC:.c=.o)

CROSS_COMPILE=
CC=gcc

all: $(TARGET)

$(TARGET) : $(OBJS)
	echo "Compiler: $(CROSS_COMPILE)$(CC)"
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	-rm -f *.o $(TARGET) *.elf *.map