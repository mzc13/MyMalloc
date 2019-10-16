all: memgrind.c mymalloc.o
	gcc memgrind.c mymalloc.o
mymalloc.o: mymalloc.c
	gcc -c mymalloc.c
clean:
	rm -f a.out mymalloc.o
