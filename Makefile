CC = gcc
CFLAGS = -Wall
TARGET = bin/make_cdn_cia
OBJS = chunkio.o cia.o main.o

ifeq ($(DEBUG),1)
	CFLAGS := -g -ggdb3
else
	CFLAGS := -O2
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(TARGET)
