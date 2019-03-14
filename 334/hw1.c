#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//TODO:
// Read line by line and access by using (i mod N)-1
// Giving lines to exec function as parameter
// Second part of the assignment

//Below comments will used for different usage

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


// #define INITIAL_CAPACITY 2
//
// void push(int *arr, int index, int value, int *size, int *capacity){
//      if(*size > *capacity){
//           realloc(arr, sizeof(arr) * 2);
//           *capacity = sizeof(arr) * 2;
//      }
//
//      arr[index] = value;
//      *size = *size + 1;
// }
//
// int main(){
//      int size = 0;
//      int capacity = INITIAL_CAPACITY;
//      int* arr = malloc(INITIAL_CAPACITY * sizeof(int));
//
//      push(arr, 0, 1, &size, &capacity);
//      push(arr, 1, 2, &size, &capacity);
//      push(arr, 2, 3, &size, &capacity);
//
//      printf("Current capacity: %d\n", capacity); // Current capacity: 2
//
//      push(arr, 3, 4, &size, &capacity);
//      push(arr, 4, 5, &size, &capacity);
//      push(arr, 5, 6, &size, &capacity);
//
//      printf("Current capacity: %d\n", capacity); // Current capacity: 16
// }


typedef struct Array{
  char *array;
	int size;
  int index;
} Array;


void close_pipe(int **fd,int i,int N){
	for(int j =0 ; j < N ; j++){
		if(j != i){
			close(fd[j][0]);
			close(fd[j][1]);
		}
	}
}

void close_pipe_2(int ***fd,int i,int N){
  for(int j =0 ; j < N ; j++){
		if(j != i){
			close(fd[j][0][0]);
			close(fd[j][0][1]);
    	close(fd[j][1][0]);
			close(fd[j][1][1]);
		}
	}
}

void close_pipe_3(int **fd,int i,int N){
	for(int j = 0 ; j < N ; j++){
		if(j != i && j != i-1){
			close(fd[j][0]);
			close(fd[j][1]);
		}
	}
}

int read_line(int N, Array* arr){

	FILE *fp;
	char * line = NULL;
	int len = 0;
	int read;

	fp = fopen("word_count/input/input.txt", "r");


	if (fp == NULL){
		exit(1);
	}

	int i = 0;

  while ((read = getline(&line, &len, fp)) != -1) {
  	arr[i].array = malloc((read)*sizeof(char));
		arr[i].size = read;
		arr[i].index = i;
    snprintf(arr[i].array, read, "%s", line);
    i++;
  }


  fclose(fp);
  if (line)
      free(line);

	return i;

}

int main(int argc, char *argv[]) {


	if(argc < 3 ){
		printf("Please enter the needed arguments \n");
		exit(1);
	}

	else if(argc == 3){


		int N = atoi(argv[1]);


		int **fd = (int **)malloc(N * sizeof(int *));;

		for(int index; index < N ; index++){
			fd[index] = (int *)malloc(2*sizeof(int));
			pipe(fd[index]);
		}


		Array *arr = (Array*)malloc(10*sizeof(Array));
		int size = read_line(N,arr);



  	for(int i = 0 ; i < N ; i++ ){
  		if(fork()){ // parent
  			close(fd[i][0]);
  			close_pipe(fd,i,N);
  			dup2(fd[i][1],1);
  			close(fd[i][1]);
				printf("\n");
				for(int k = 0 ; k < size ; k++ ){
  				if(i % N == arr[k].index){ // or 1
  					fprintf(stdout,arr[k].array); // check
  					fflush(stdout); // check
  				}
  			}

  		}
  		else{ // mapper
  			close(fd[i][1]);
  			close_pipe(fd,i,N);
  			dup2(fd[i][0],0);
  			close(fd[i][0]);
				execl(argv[2],stdin,(char *)0);
  		}
  	}

  }
	else if(argc == 4){

  	int N = atoi(argv[1]);

  	int ***fd = (int ***)malloc(N * sizeof(int **));;

  	for(int index; index < N ; index++){
  		fd[index] = (int **)malloc(2*sizeof(int *));
      for(int index2 ; index2 < 2 ; index2++){
        fd[index][index2] = (int *)malloc(2*sizeof(int));
        pipe(fd[index][index2]);
      }
  	}

    int **fd2 = (int **)malloc(N * sizeof(int *));;

    for(int index; index < N ; index++){
      fd2[index] = (int *)malloc(2*sizeof(int));
      pipe(fd[index]);
    }


    // 0 for Mapper, 1 for Reducer

    for(int i = 0 ; i < N ; i++ ){
      if(fork()){

        if(fork()){ // parent
          close(fd[i][0][0]);
          close(fd[i][1][0]);
          close(fd[i][1][1]);
          close_pipe_2(fd,i,N);
          dup2(fd[i][0][1],1);
          close(fd[i][0][1]);

        }
        else{ // mapper
          close(fd[i][0][1]);
          close(fd[i][1][0]);
          close_pipe_2(fd,i,N);
          dup2(fd[i][1][1],1);
          dup2(fd[i][0][0],0);
          close(fd[i][0][0]);
          execl(argv[2],stdin,(char *)0);
        }

      }
      else{ // reducer
        if(i == 0){

          close(fd[i][0][0]);
          close(fd[i][0][1]);
          close(fd[i][1][1]);
          close(fd2[i][0]);
          close_pipe(fd2,i,N);
          close_pipe_2(fd,i,N);
          dup2(fd[i][1][0],0);
          dup2(fd2[i][1],1)
          close(fd[i][0][1]);
          close(fd2[i][1]);
          execl(argv[3],stdin,(char *)0);

        }

        else if( i != 0 && i == N-1){

          close(fd[i][0][0]);
          close(fd[i][0][1]); // Parent pipe closed.
          close(fd[i][1][1]);
          close_pipe_2(fd,i,N);
          close(fd2[i-1][1]);
          close(fd2[i][0]);
          close_pipe_3(fd2,i,N);
          dup2(fd2[i-1][0],2); // Read end to stderr
          dup2(fd2[i][1],1);
          dup2(fd[i][1][0],0);
          close(fd2[i-1][0]);
          close(fd2[i][1]);
          close(fd[i][1][0]);
          execl(argv[3],stdin,(char *)0);
        }
        else{ // i = N-1 case
          close(fd[i][0][0]);
          close(fd[i][0][1]); // Parent pipe closed.
          close(fd[i][1][1]);
          close_pipe_2(fd,i,N);
          close(fd2[i-1][1]);
          close_pipe(fd2,i-1,N);
          dup2(fd2[i-1][0],2); // Read end to stderr
          dup2(fd[i][1][0],0);
          close(fd2[i-1][0]);
          execl(argv[3],stdin,(char *)0);
        }
      }
    }





	}
	else{
		printf("Too many argumants!! \n");
		exit(2);
	}

	//
	// pipe(fd1);
	// pipe(fd2);
	//
	// if (p=fork()) {
	//
	// 	if(q=fork()){
	//
	// 		close(fd1[0]);
	// 		close(fd2[0]);
	// 		close(fd2[1]);
	// 		dup2(fd1[1],2);
	// 		close(fd1[0]);
	//
	// 		for (i=0;i<3;i++) {
	// 			fprintf(stderr,messages[i]);
	// 			fflush(stderr);
	// 			sleep(1);
	// 		}
	//
	//
	// 	}
	// 	else{
	// 		close(fd1[1]);
	// 		close(fd2[0]);
	// 		dup2(fd1[0],0);
	// 		dup2(fd2[1],1);
	// 		close(fd1[0]);
	// 		close(fd2[1]);
	// 		execl("/usr/bin/bc","bc","-q",(char *)0);
	//
	//
	// 	}
	//
	//
	// } else {
	// 	close(fd1[0]);
	// 	close(fd1[1]);
	// 	close(fd2[1]);
	// 	dup2(fd2[0],0);
	// 	close(fd2[0]);
	//
	// 	for(i=0;i<3;i++){
	// 		if (fgets(result,100,stdin)) {
	// 			printf("%s\n",result);
	// 		}
	// 	}
	//
	// }

	return 0;
}
