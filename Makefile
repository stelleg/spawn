cc=gcc
cflags=-g -O2 -std=c99 -lpthread
all: fib hello

%:%.c spawn.h
	${cc} ${cflags} $< -o $@


