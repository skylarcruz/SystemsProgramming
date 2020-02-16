GET_WORD := getWord/
DICT := dict/

wordpairs: getWordPairs.o hashTable.o crc64.o getWord.o
	cc -o wordpairs getWordPairs.o hashTable.o crc64.o getWord.o -g
	rm hashTable.o
	rm getWordPairs.o
	rm crc64.o
	rm getWord.o

getWordPairs.o: getWordPairs.c hashTable.c $(GET_WORD)include/getWord.c
	cc -c getWordPairs.c hashTable.c $(GET_WORD)include/getWord.c -I $(GET_WORD)include -I $(DICT)

hashTable.o: hashTable.c $(DICT)crc64.c $(DICT)crc64.h
	cc -c hashTable.c $(DICT)crc64.c -I $(DICT)

crc64.o: $(DICT)crc64.c $(DICT)crc64.h
	cc -c $(DICT)crc64.c -I $(DICT)

getWord.o: $(GET_WORD)include/getWord.c
	cc -c $(GET_WORD)include/getWord.c -I $(GET_WORD)include

clean:
	rm wordpairs