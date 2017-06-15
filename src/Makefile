all: spawn.so spawn.a

spawn.so: spawn.o
	gcc -shared -o libspawn.so spawn.o

spawn.a: spawn.o
	ar cr spawn.a spawn.o

spawn.o: spawn.c spawn.h
	gcc -c -fPIC spawn.c

clean:
	rm =f *.o *.a *.so
