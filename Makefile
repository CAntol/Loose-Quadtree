CFLAGS = -ansi -Wall
PROGS = loosequadtree printquad showquad

all: $(PROGS)

.PHONY: all clean

loosequadtree:	loosequadtree.c
	gcc -o loosequadtree loosequadtree.c -lm
	
showquad:	showquad.c
	gcc -o showquad showquad.c -L/usr/local/X11/lib64 -lX11 -lm

printquad:	printquad.c
	gcc -o printquad printquad.c
	
clean:
	 rm -f *.o $(PROGS) part4.4.tar.gz
	 
tar:
	tar -czf part4.4.tar.gz drawing* loosequadtree.c Makefile printquad.c showquad.c
	
scp:
	scp part4.4.tar.gz cantol@linux.grace.umd.edu:/homes/cantol/420project1/part4.4.tar.gz
	
submit:
	submit 2011 fall cmsc 420 0101 4 part4.4.tar.gz
