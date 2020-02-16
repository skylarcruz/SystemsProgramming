prodCon: prodCon.o
	cc -pthread -o prodCon prodCon.o -g -lm
	rm prodCon.o

prodCon.o: prodCon.c
	cc -c prodCon.c

all: prodCon prodCon.o

clean:
	rm prodCon

run: prodCon
	./prodCon