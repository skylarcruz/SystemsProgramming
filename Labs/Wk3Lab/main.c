#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void reversePrint(int f);

int main(int argc, char *argv[]){

	int fd;
	if (open(argv[1], O_RDONLY, 0) > -1){
		fd = open(argv[1], O_RDONLY, 0);

		char buffer[1];
		lseek(fd, -1, SEEK_END);
		int n;

		while (lseek(fd, 0, SEEK_CUR) > 0) {
			n = read(fd, buffer, 1);
			if (n == 1) {
				write(1, buffer, 1);
				lseek(fd, -2, SEEK_CUR);
			}
		}
		n = read(fd, buffer, 1);
		if (n == 1) {
			write(1, buffer, 1);
			lseek(fd, -2, SEEK_CUR);
		}

		close(fd);

	}

	else {
		printf("File Open Error");
		return 0;
	}

	printf("\n\n=================Part 2=================\n\n");
	
	if (open(argv[1], O_RDONLY, 0) > -1){
		fd = open(argv[1], O_RDONLY, 0);

		reversePrint(fd);

		close(fd);

	}

	else {
		printf("File Open Error");
		return 0;
	}

	return 0;

}

void reversePrint(int f){
	int n;
	char buff[1];
	n = read(f, buff, 1);

	if (n > 0){
		reversePrint(f);
	}
	write(1, buff, 1);
}