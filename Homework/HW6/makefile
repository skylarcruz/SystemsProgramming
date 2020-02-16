philo: philo.o
	cc -o philo philo.o -g -lm
	rm philo.o

philo.o: philo.c
	cc -c philo.c

all: philo philo.o

clean:
	rm philo

run: philo
	./philo