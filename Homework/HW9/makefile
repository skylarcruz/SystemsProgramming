dayserve: dayserve.o
	cc -o dayserve dayserve.o
	rm dayserve.o

dayserve.o: dayserve.c
	cc -c dayserve.c

daytime: daytime.o
	cc -o daytime daytime.o
	rm daytime.o

daytime.o: daytime.c
	cc -c daytime.c

all: dayserve dayserve.o daytime daytime.o

clean:
	rm dayserve daytime

