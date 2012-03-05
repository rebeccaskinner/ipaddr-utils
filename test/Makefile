CC=gcc

CFLAGS= -std=gnu99 \
	-O3 \
	-Wall \
	-Wextra \
	-Werror \
	-Wno-unused \
	-g \
	-ggdb \
	-mtune=native \
	-ftree-vectorizer-verbose=5\
	-ftree-vectorize\
	-flto

LFLAGS= -flto

SOURCES=$(shell echo *.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
MAKEADDR_OBJ=$(filter-out makenoncolliding.o loadaddrs.o bftest.o,$(OBJECTS))
LOADADDR_OBJ=$(filter-out makenoncolliding.o makeaddrs.o bftest.o,$(OBJECTS))
NONCOLLIDING_OBJ=$(filter-out loadaddrs.o bftest.o makeaddrs.o,$(OBJECTS))
BFTEST_OBJ=$(filter-out makenoncolliding.o loadaddrs.o makeaddrs.o,$(OBJECTS))

all: makeaddrs loadaddrs bftest makenoncolliding

%.o:%.c
	$(CC) -c $(CFLAGS) $(^) -o $(@)

makeaddrs:$(MAKEADDR_OBJ)
	$(CC) $(LFLAGS) $(MAKEADDR_OBJ) -o $(@) 

loadaddrs:$(LOADADDR_OBJ)
	$(CC) $(LFLAGS) $(LOADADDR_OBJ) -o $(@) 

bftest:$(BFTEST_OBJ)
	$(CC) $(LFLAGS) $(BFTEST_OBJ) -o $(@)

makenoncolliding:$(NONCOLLIDING_OBJ)
	$(CC) $(LFLAGS) $(NONCOLLIDING_OBJ) -o $(@)

clean:
	-rm -f *.o
	-rm -f makeaddrs
	-rm -f loadaddrs
	-rm -f bftest
	-rm -f makenoncolliding
	-rm -f core*
	-rm -f *.s

.PHONY: all clean
