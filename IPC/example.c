#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){


  int N = atoi(argv[1]);

  int fd[2];

  if(fork()){
    close(fd[0]);
    dup2(fd[1],1);
    close(fd[1]);
    fprintf(stdout,"Hello@!\n"); // check
    fflush(stdout); // check
  }
  else{
    close(fd[1]);
    // dup2(fd[0],0);
    close(fd[0]);

  }

}
