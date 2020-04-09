TARGET	= server
OBJECT	= server.c
CC 	= gcc
CFLAGS	= -lpthread

all: $(TARGET)

$(TARGET): server.c
	$(CC) -o $(TARGET) $^ $(CFLAGS)

.PHONY: clean

clean:
	$(RM) -f $(TARGET)
