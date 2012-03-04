CC=gcc

CFLAGS= -std=gnu99 \
	-O0 \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-g \
	-ggdb
	# -mtune=native \
	# -flto

LFLAGS= -flto

SOURCES=$(shell echo *.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
MAKEADDR_OBJ=$(filter-out loadaddrs.o,$(OBJECTS))
LOADADDR_OBJ=$(filter-out makeaddrs.o,$(OBJECTS))

all: makeaddrs loadaddrs

%.o:%.c
	$(CC) -c $(CFLAGS) $(^) -o $(@)

makeaddrs:$(MAKEADDR_OBJ)
	$(CC) $(LFLAGS) $(MAKEADDR_OBJ) -o $(@) 

loadaddrs:$(LOADADDR_OBJ)
	$(CC) $(LFLAGS) $(LOADADDR_OBJ) -o $(@) 

clean:
	-rm -f *.o
	-rm -f makeaddrs
	-rm -f loadaddrs
	-rm -f core*

.PHONY: all clean
