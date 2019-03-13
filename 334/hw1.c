#include<unistd.h>
#include<stdio.h>

//TODO:
// Read line by line and access by using (i mod N)-1
// Giving lines to exec function as parameter
// Second part of the assignment

//Below comments will used for different usage 

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


// typedef struct {
//   int *array;
//   size_t used;
//   size_t size;
// } Array;
//
// void initArray(Array *a, size_t initialSize) {
//   a->array = (int *)malloc(initialSize * sizeof(int));
//   a->used = 0;
//   a->size = initialSize;
// }
//
// void insertArray(Array *a, int element) {
//   // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
//   // Therefore a->used can go up to a->size
//   if (a->used == a->size) {
//     a->size *= 2;
//     a->array = (int *)realloc(a->array, a->size * sizeof(int));
//   }
//   a->array[a->used++] = element;
// }
//
// void freeArray(Array *a) {
//   free(a->array);
//   a->array = NULL;
//   a->used = a->size = 0;
// }

void close_pipe(int **fd,int i,int N){
	for(int j =0 ; j < N ; j++){
		if(j != i){
			close(fd[j][0]);
			close(fd[j][1]);
		}
	}
}

char* read_line(int N){
	FILE *fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("/word_count/input/input.txt", "r");



  while ((read = getline(&line, &len, fp)) != -1) {
      printf("Retrieved line of length %zu:\n", read);
      printf("%s", line);
  }

  fclose(fp);
  if (line)
      free(line);
  exit(EXIT_SUCCESS);
}

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

		if (fp == NULL)
				exit(EXIT_FAILURE);


		for(j = 0 ; j < N ; j++ ){
			if(fork()){
				close(fd[i][0]);
				close_pipe(fd,i,N);
				dup2(fd[i][1],1);
				close(fd[i][1]);
			}
			else{
					close(fd[i][1]);
					close_pipe(fd,i,N);
					dup2(fd[i][0],0);
					close(fd[i][0]);
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
