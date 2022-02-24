CFLAGS=-Wall
all:
	gcc	-o	memsim	memsim.c	fifo.c	lru.c	vms.c	dataStructures.c
