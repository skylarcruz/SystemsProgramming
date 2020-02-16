readable: readable.o
	cc -o readable readable.o -g
	rm readable.o

readable.o: readable.c
	cc -c readable.c

all: readable readable.o

clean:
	rm readable

# run by setting ARGS to desired
# directory in command line with:
# 'make run ARGS="pathname"'
run: readable
	./readable $(ARGS)