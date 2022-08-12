#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

char buf[512];

int
readargs(char** args)
{
  char *bufp = buf;
  char *end = buf + sizeof(buf);
  /* Read line */
  while (bufp != end-1 && read(0, bufp, 1) == 1) {
    if (*bufp == '\n') {
      break;
    }
    bufp++;
  }
  
  *bufp = '\0';
  end = bufp;
  bufp = buf;

  /* Split arguments */
  *args = 0;
  while (bufp != end) {
    if (*args == 0 && *bufp != ' ' && *bufp != '\t') {
      *args = bufp;
    } else if (*args != 0 && (*bufp == ' ' || *bufp == '\t')) {
      *bufp = '\0';
      *(++args) = 0;
    }
    bufp++;
  }

  *(++args) = 0;

  return end - buf;
}

int
xargs(char* prog, char** args)
{
  int status = 0;
  if (fork() > 0) {
    wait(&status);
  } else {
    exec(prog, args);
  }

  return status;
}

int
main(int argc, char* argv[])
{
  int i;
  char *prog;
  char *args[MAXARG];

  if (argc < 2) {
    fprintf(2, "xargs: program name required\n");
    exit(1);
  }

  prog = argv[1];
  for (i = 1; i < argc; i++) {
    args[i-1] = argv[i];
  }
  
  while (readargs(&args[argc-1]) != 0) {
    xargs(prog, args);
  }
  
  exit(0);
}
