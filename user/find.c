#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

char*
filename(char *path)
{
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--);
  p++;

  return p;
}

void
find(char* path, char* name)
{
  int fd;
  char buf[512], *bufp;
  struct dirent entry;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    exit(1);
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    exit(1);
  }

  switch (st.type) {
  case T_FILE:
    if (strcmp(filename(path), name) == 0) {
      printf("%s\n", path);
    }
    break;
  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      fprintf(2, "find: path too long\n");
      exit(1);
    }

    if (strcmp(filename(path), name) == 0) {
      printf("%s\n", path);
    }
    
    strcpy(buf, path);
    bufp = buf + strlen(buf);
    *bufp++ = '/';
    while (read(fd, &entry, sizeof(entry)) == sizeof(entry)) {
      if (entry.inum == 0)
        continue;

      memmove(bufp, entry.name, DIRSIZ);
      bufp[DIRSIZ] = 0;

      if (strcmp(filename(buf), ".") == 0 || strcmp(filename(buf), "..") == 0)
        continue;
      
      find(buf, name);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(2, "find: path and filename expected\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}
