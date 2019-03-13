#include<unistd.h>
#include<stdio.h>

//
// #define _GNU_SOURCE
// #include <stdio.h>
// #include <stdlib.h>
//
// int main(void)
// {
//     FILE * fp;
//     char * line = NULL;
//     size_t len = 0;
//     ssize_t read;
//
    // fp = fopen("/etc/motd", "r");
    // if (fp == NULL)
    //     exit(EXIT_FAILURE);
//
//     while ((read = getline(&line, &len, fp)) != -1) {
//         printf("Retrieved line of length %zu:\n", read);
//         printf("%s", line);
//     }
//
//     fclose(fp);
//     if (line)
//         free(line);
//     exit(EXIT_SUCCESS);
// }


void close_pipe(int **fd,int i,int N){
	for(int j =0 ; j < N ; j++){
		if(j != i){
			close(fd[j][0]);
			close(fd[j][1]);
		}
	}
}

void read_line(	);

int main(int argc, int argv[]) {

	int p,i,q;
	int fd1[2],fd2[2];
	char *messages[]={"5+4\n","5*6\n","23+100-(4*4)\n"};

	if(argc < 3 ){
		printf("Please enter the needed arguments");
		exit(1);
	}

	else if(argc == 3){

		int N = argv[2];

		int **fd = (int **)malloc(N * sizeof(int *));;

		for(int index; index < N ; index++){
			fd[index] = (int *)malloc(2*sizeof(int));
		}


		FILE *fp;
		char * line = NULL;
		size_t len = 0;
		ssize_t read;

		fp = fopen("/etc/motd", "r");
		if (fp == NULL)
				exit(EXIT_FAILURE);

		for(j = 0 ; j < N ; j++ ){
			if(fork()){
				close(fd[i][0]);
				close_pipe(fd,i,N);
				dup2(fd[i][1],1);
			}
			else{
					close(fd[i][1]);
					close_pipe(fd,i,N);
					dup2(fd[i][0],0);
			}
		}

	}
	else if(argc == 4){
	}
	else{
		printf("Too many argumants!!")
		exit(2);
	}


	pipe(fd1);
	pipe(fd2);

	if (p=fork()) {

		if(q=fork()){

			close(fd1[0]);
			close(fd2[0]);
			close(fd2[1]);
			dup2(fd1[1],2);
			close(fd1[0]);

			for (i=0;i<3;i++) {
				fprintf(stderr,messages[i]);
				fflush(stderr);
				sleep(1);
			}


		}
		else{
			close(fd1[1]);
			close(fd2[0]);
			dup2(fd1[0],0);
			dup2(fd2[1],1);
			close(fd1[0]);
			close(fd2[1]);
			execl("/usr/bin/bc","bc","-q",(char *)0);


		}


	} else {
		close(fd1[0]);
		close(fd1[1]);
		close(fd2[1]);
		dup2(fd2[0],0);
		close(fd2[0]);

		for(i=0;i<3;i++){
			if (fgets(result,100,stdin)) {
				printf("%s\n",result);
			}
		}

	}

	return 0;
}
