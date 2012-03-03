CC=gcc

CFLAGS= -std=gnu99 \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-mtune=native \
	-flto \
	-O3

LFLAGS= -flto

SOURCES=$(shell echo *.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

all: makeaddrs

%.o:%.c
	$(CC) -c $(CFLAGS) $(^) -o $(@)

makeaddrs:$(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $(@) 

clean:
	-rm -f *.o
	-rm -f makeaddrs

.PHONY: all clean
