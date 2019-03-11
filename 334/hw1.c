/* parent master -> child slave (bc -q, a simple calculator)
   two pipes one for parent to child, other from child to parent
   parent -> 3+4, 5*6 * ... child bc -q results -> parent
*/
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {


	printf("I reached 17");
	int p,i;
	int fd1[2],fd2[2];
	char *messages[]={"3+4\n","5*6\n","10+100-(4*4)\n"};
	char buffer[100];
	char readbuffer[100];

	printf("I reached 22");

	pipe(fd1);
	pipe(fd2);

	if (p=fork()) { //Parent process

		close(fd2[0]); // Read end of the second pipe closed
		close(fd2[1]); // Write end of the second pipe closed
		close(fd1[0]); // Read end of the first pipe closed

		dup2(fd1[1],1); // Duplicate write of the first pipe to stdout

		printf("I reached 33");


		fd1[1] = open("./word_count/input/input.txt", O_RDONLY | O_WRONLY);

		if(fd1[1] < 0){
			perror("Not opened"); exit(1);
		}

		while(i){
				i = read(fd1[1],buffer,10);
		}

		printf("I reached 45");
		write(fd1[1], buffer, (strlen(buffer) + 1));
		close(fd1[1]); // Write end of the first pipe closed

		waitpid(p,NULL,0);

	}
	else {

		close(fd1[1]); // Write end of the first pipe closed
		close(fd2[0]); // Read end of the second pipe closed

    int nbytes = read(fd1[0], readbuffer, sizeof(readbuffer));

		dup2(fd1[0],0); // Duplicate read end of the first pipe to stdin
		dup2(fd2[1],1); // Duplicate write end of the second pipe to stdout

		fd2[1] = open("/dev/pts/22", O_RDONLY | O_WRONLY);

		printf("I reached ");

		close(fd1[0]);
		close(fd2[1]); // Write end of the second pipe closed

		execv("./word_count/src/WC_Mapper",readbuffer);

	}


	return 0;
}
