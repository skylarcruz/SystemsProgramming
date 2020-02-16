all: mftp mftp.o mftpserve mftpserve.o

mftp: mftp.o
	cc -o mftp mftp.o
	rm mftp.o

mftpserve: mftpserve.o
	cc -o mftpserve mftpserve.o
	rm mftpserve.o

mftp.o: mftp.c mftp.h
	cc -c mftp.c mftp.h

mftpserve.o: mftpserve.c mftp.h
	cc -c mftpserve.c mftp.h

clean:
	rm mftp mftpserve