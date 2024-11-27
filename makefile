all: mymalloc.exe

mymalloc.exe: mymalloc.o
	gcc -o mymalloc.exe mymalloc.o

mymalloc.o: mymalloc.c
	gcc -c mymalloc.c

clean:
	rm mymalloc.o mymalloc.exe
