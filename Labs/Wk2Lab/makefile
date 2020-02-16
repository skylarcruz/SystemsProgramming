PeopleApp: main.o People.o
	cc -o PeopleApp main.o  People.o People.h
	rm main.o
	rm People.o

main.o: main.c
	cc -c main.c

People.o: People.c
	cc -c People.c

clean:
	rm PeopleApp