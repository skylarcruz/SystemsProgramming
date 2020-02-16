GET_WORD := getWord/


sortThreads: main.o getWord.o sort.o
	cc -pthread -o sortThreads main.o getWord.o sort.o -g -lm
	rm getWord.o main.o sort.o

main.o: main.c sort.c $(GET_WORD)include/getWord.c
	cc -c main.c sort.c $(GET_WORD)include/getWord.c -I $(GET_WORD)include

getWord.o: $(GET_WORD)include/getWord.c
	cc -c $(GET_WORD)include/getWord.c -I $(GET_WORD)include

sort.o: sort.c sort.h
	cc -c sort.c

all: sortThreads main.o sort.o

clean:
	rm sortThreads