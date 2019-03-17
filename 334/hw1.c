#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//TODO:
// Read line by line and access by using (i mod N)-1
// Giving lines to exec function as parameter
// Second part of the assignment


typedef struct Array{
  char *array;
	int size;
  int index;
} Array;


void append(char* s, char c)
{
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}


void close_pipe(int **fd,int i,int N){
	for(int j = 0 ; j < N ; j++){
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

	char * line = NULL;
	size_t len = 0;
	size_t read;

	int i = 0;

  while ((read = getline(&line, &len, stdin)) != -1) {
    // printf("%s",line);
    arr[i].array = malloc((read)*sizeof(char));
		arr[i].size = read;
		arr[i].index = i;
    strncpy(arr[i].array,line,read);
    append(arr[i].array,' ');
    // snprintf(arr[i].array, read, "%s\n", line);
    i++;
  }


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

    Array *arr = (Array*)malloc(10*sizeof(Array));
    int size = read_line(N,arr);

		int **fd = (int **)malloc(N * sizeof(int *));

		for(int index = 0; index < N ; index++){
			fd[index] = (int *)malloc(2*sizeof(int));
      pipe(fd[index]);
		}
  	for(int i = 0 ; i < N ; i++ ){
      if(fork()){
  			close(fd[i][0]);
        for(int j = 0 ; j < N ; j++){
      		if(j != i){
      			close(fd[j][0]);
      			close(fd[j][1]);
      		}
      	}
        dup2(fd[i][1],1);
  			close(fd[i][1]);
				printf("\n");
        char * temp;
        char * out ;
        int t_size = 0;
				for(int k = 0 ; k < size ; k++ ){
  				if(arr[k].index % N == i){ // or 1
            if(k == 0){
              out = (char*)malloc((arr[k].size)*sizeof(char));
              strcat(out,arr[k].array);
              t_size += arr[k].size;
            }
            else{
              temp = (char*)malloc((strlen(out)+arr[k].size)*sizeof(char));
              strcpy(temp,out);
              out = (char*)malloc((strlen(temp))*sizeof(char));
              strcat(temp,arr[k].array);
              strcpy(out,temp);
              t_size = (strlen(temp)+arr[k].size);
              free(temp);
              fflush(stdout); // check
            }

  				}
  			}
        write(1,out,t_size); // check
        free(out);
  		}
  		else{
        close(fd[i][1]);
        for(int j = 0 ; j < N ; j++){
          if(j != i){
            close(fd[j][0]);
            close(fd[j][1]);
          }
        }
  			dup2(fd[i][0],0);
  			close(fd[i][0]);
        fflush(stdout);
        execv(argv[2],NULL);
      }
  	}
    //
    // int status;
    // while(wait(&status) > 0 ){
    // }

    for (size_t i = 0; i < N; i++) {
      free(fd[i]);
    }
    free(fd);
    free(arr);



  }
	else if(argc == 4){

  	int N = atoi(argv[1]);

    Array *arr = (Array*)malloc(10*sizeof(Array));
    int size = read_line(N,arr);


  	int ***fd = (int ***)malloc(N * sizeof(int **));;

  	for(int index = 0; index < N ; index++){
  		fd[index] = (int **)malloc(2*sizeof(int *));
      for(int index2 = 0 ; index2 < 2 ; index2++){
        fd[index][index2] = (int *)malloc(2*sizeof(int));
        pipe(fd[index][index2]);
      }
  	}

    int **fd2 = (int **)malloc(N * sizeof(int *));;

    for(int index = 0; index < N ; index++){
      fd2[index] = (int *)malloc(2*sizeof(int));
      pipe(fd2[index]);
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
          printf("\n");
  				for(int k = 0 ; k < size ; k++ ){
    				if(i % N == arr[k].index){ // or 1
    					write(1,arr[k].array,arr[k].size); // check
    					fflush(stdout); // check
    				}
    			}


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
          dup2(fd2[i][1],1);
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


    int status;
    while(wait(&status) > 0 );


    for (size_t i = 0; i < N; i++) {
      for (size_t j = 0; j < 2; j++) {
        free(fd[i][j]);
      }
      free(fd[i]);
    }
    free(fd);

    for (size_t i = 0; i < N; i++) {
      free(fd2[i]);
    }
    free(fd2);
    free(arr);



	}
	else{
		printf("Too many argumants!! \n");
		exit(2);
	}


	return 0;
}
