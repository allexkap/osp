CC=gcc
CFLAGS=-O2 -Wall -Wextra -Werror
LDFLAGS=
EXECUTABLE=lab12aekN32491
SOURCES=main.c nrftw.c

OBJECTS=$(SOURCES:.c=.o)

.PHONY: all clean


all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
