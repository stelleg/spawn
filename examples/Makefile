cc=gcc
include=-I../src
ldflags=-lpthread
cflags=-g -Wall -O2 -std=c99 
examples=fib hello

all: ${examples}

%:%.c spawn.o
	${cc} ${cflags} ${include} ${ldflags} spawn.o $< -o $@

spawn.o: ../src/spawn.c ../src/spawn.h
	gcc ${cflags} -c ../src/spawn.c

clean:
	rm -f ${examples} spawn.o
