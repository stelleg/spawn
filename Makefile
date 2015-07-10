cc=gcc
cflags=-g -Wall -O2 -std=c99 -lpthread
examples=fib hello

all: ${examples}

%:%.c spawn.h
	${cc} ${cflags} $< -o $@

clean:
	rm -f ${examples}
