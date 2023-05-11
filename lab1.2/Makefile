CC=gcc
CFLAGS=-O2 -Wall -Wextra -Werror
LDFLAGS=-ldl
EXECUTABLE=lab12aekN32491
SOURCES=main.c nrftw.c plugin_handler.c xalloc.c
PLUGINS=libparity.c libfreq.c libipv4.c

OBJECTS=$(SOURCES:.c=.o)
SHARED=$(PLUGINS:.c=.so)

.PHONY: all clean


all: $(EXECUTABLE) $(SHARED)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.so: %.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(OBJECTS) $(SHARED)
