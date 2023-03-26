CC=gcc
CFLAGS=-O2 -Wall -Wextra -Werror
LDFLAGS=
EXECUTABLE=lab12aekN32491
SOURCES=main.c nrftw.c plugin_handler.c
PLUGINS=libparity.c libfreq.c

OBJECTS=$(SOURCES:.c=.o)
SHARED=$(PLUGINS:.c=.so)

.PHONY: all clean


all: $(EXECUTABLE) $(SHARED)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.so: %.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(OBJECTS) $(SHARED)
