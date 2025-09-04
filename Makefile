CFLAGS = -Wextra -Wall -pedantic -std=gnu99 -g

all: clean stmp

stmp:
	gcc -c $(CFLAGS) stmp.c -o stmp.o

clean:
	rm stmp.o libstmp
