warn: warn.o
	cc -o warn warn.o -g
	rm warn.o

warn.o: warn.c
	cc -c warn.c

all: warn warn.o

clean:
	rm warn

run: warn
	./warn