#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

bool is_same_file(const char *lhs_path, const char *rhs_path) {
  struct stat stat1, stat2;
  if (stat(lhs_path, &stat1) == -1 || stat(rhs_path, &stat2) == -1) {
    perror("error when retrieving file information");
    return false;
  }
  return (stat1.st_ino == stat2.st_ino && stat1.st_dev == stat2.st_dev);
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    perror("too little argument");
    exit(EXIT_FAILURE);
  }
  const char *path1 = argv[1];
  const char *path2 = argv[2];
  if (is_same_file(path1, path2)) {
    printf("yes\n");
  } else {
    printf("no\n");
  }
  return 0;
}
