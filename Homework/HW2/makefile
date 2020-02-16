ARGS := "abc"

ok: main.o
	cc -o ok ok.o
	rm ok.o

main.o: ok.c
	cc -c ok.c

all: ok ok.o

clean:
	rm ok

# run by setting ARGS to desired search word
# can also overide ARGS in command line with:
# make run ARGS="SEARCHWORD"
run: ok
	./ok $(ARGS)