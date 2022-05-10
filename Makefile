CC = gcc
CFLAGS = -g -Wall -pthread

all: bin/sync

bin/sync: obj/ bin/ obj/sync.o
	$(CC) $(CFLAGS) obj/sync.o -o $@

obj/sync.o: sync.h sync.c
	$(CC) $(CFLAGS) sync.c -c -o $@

bin/:
	mkdir -p $@

obj/:
	mkdir -p $@

clean:
	rm -rf bin/ obj/