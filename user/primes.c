#include "kernel/types.h"
#include "user/user.h"

int fd[2];

int
sieve(void) {
  int status;
  int num, pass;
  int in_fd, out_fd;

  do {
    /* Store fd which we will use,
     * and close fd which we wont.
     * Otherwise we'll have duplicated
     * fds leading to fd leakage and
     * non-closing pipes */
    in_fd = fd[0];
    close(fd[1]);
    
    if (!read(in_fd, (char*)&num, 4)) {
      exit(0);
    }

    printf("prime %d\n", num);

    /* Create new pipe and store its fd */
    pipe(fd);
    out_fd = fd[1];
    
  } while (fork() == 0);

  /* parent -- pass numbers */
  while (read(in_fd, (char*)&pass, 4)) {
    if (pass % num) {
      write(out_fd, (char*)&pass, 4);
    }
  }
  close(out_fd);
  wait(&status);
  exit(status);
}

int
main(int argc, char *argv[]) {
  int i, out_fd;
  int status = 0;

  pipe(fd);
  out_fd = fd[1];
  
  if (fork() == 0) {
    // child -- sieve
    sieve();
  }

  // parent -- send numbers
  for (i = 2; i < 35; i++) {
    write(out_fd, (char*)&i, 4);
  }
  close(out_fd);
  wait(&status);
  exit(status);
}
