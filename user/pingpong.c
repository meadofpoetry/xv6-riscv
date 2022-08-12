#include "kernel/types.h"
#include "user/user.h"

int fd[2];

void
parent(void) {
  char byte = 'x';
  int pid = getpid();

  if (write(fd[1], &byte, 1) != 1) {
    fprintf(2, "pingpong: writing error\n");
    exit(1);
  }

  if (read(fd[0], &byte, 1) != 1) {
    fprintf(2, "pingpong: reading error\n");
    exit(1);
  }

  printf("%d: received pong\n", pid);
}

void
child(void){
  char byte;
  int pid = getpid();

  if (read(fd[0], &byte, 1) != 1) {
    fprintf(2, "pingpong: reading error\n");
    exit(1);
  }

  printf("%d: received ping\n", pid);

  if (write(fd[1], &byte, 1) != 1) {
    fprintf(2, "pingpong: writing error\n");
    exit(1);
  }
}

int
main(int argc, char* argv[]){
  int pid;

  if(pipe(fd)){
    fprintf(2, "pingpong: pipe creation failure\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    fprintf(2, "pingpong: fork failed\n");
    exit(1);
  } else if(pid == 0){
    parent();
  } else {
    child();
  }
  
  exit(0);
}
