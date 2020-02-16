connect: connect.o
	cc -o connect connect.o -g
	rm connect.o

connect.o: connect.c
	cc -c connect.c

all: connect connect.o

clean:
	rm connect

# run by setting ARGS
# in command line with:
# 'make run ARGS="pathname"'
run: connect
	./connect $(ARGS)