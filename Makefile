CFLAGS = -Wextra -Wall -pedantic -std=gnu99 -g

all: clean stmp libstmp

stmp:
	gcc -c $(CFLAGS) stmp.c -o stmp.o

libstmp:
	gcc $(CFLAGS) libstmp.c stmp.o -o libstmp

clean:
	rm stmp.o libstmp
