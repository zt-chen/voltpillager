CFLAGS =  -Wall -O0
LFLAGS = -lncurses

all: operation 
	objdump -D operation -M intel | grep -C80 \<compare_it\>:

operation: operation.o
	gcc $(CFLAGS) -S -c -o operation.asm operation.c -mrdrnd
	gcc $(CFLAGS) -o operation operation.o -lpthread $(LFLAGS)

operation.o: operation.c
	gcc $(CFLAGS) -c -o operation.o operation.c -mrdrnd


clean:
	rm *.o
	rm operation
