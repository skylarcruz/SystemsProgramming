threads: threads.o
	cc -pthread -o threads threads.o -g -lm
	rm threads.o

threads.o: threads.c
	cc -c threads.c

all: threads threads.o

clean:
	rm threads

run: threads
	./threads