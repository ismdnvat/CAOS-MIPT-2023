#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <getopt.h>

void rm_dir(const char *path) {
  rmdir(path);
}
void rm_slink(const char *path) {
  unlink(path);
}

void rm_recursive(const char *path, int flag_rm_dir) {
  if (flag_rm_dir == 0) {
    perror("rm: cannot delete: this is a directory and you dont have flag -r");
    exit(EXIT_FAILURE);
  }
  DIR *dir = opendir(path);
  if (dir == NULL) {
    perror("rm: cannot open");
    exit(EXIT_FAILURE);
  }
//  printf("we opened dir. URAAA\n");
  struct dirent *dirent = NULL;
  while ((dirent = readdir(dir))) {
    if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) {
      continue;
    }
    char full_path[PATH_MAX];
    strcpy(full_path, path);
    strcat(full_path, "/");
    strcat(full_path, dirent->d_name);
    struct stat stat_file;
    if (stat(full_path, &stat_file) != 0) {
      perror("error");
      exit(EXIT_FAILURE);
    }
    if (S_ISDIR(stat_file.st_mode)) {
//      printf("it is dir %s\n", full_path);
      rm_recursive(full_path, flag_rm_dir);
    } else {
      unlink(full_path);
    }
  }
  closedir(dir);
  rm_dir(path);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "rm: missing operand\n");
    exit(EXIT_FAILURE);
  }
  int opt = 0;
  int flag_rm_dir = 0;
  while ((opt = getopt(argc, argv, "r:")) != -1) {
    switch (opt) {
      case '?':
        perror("rm: missing argument");
        exit(EXIT_FAILURE);
      case 'r':
        flag_rm_dir = 1;
      default:
        break;
    }
  }
  struct stat stat_file;
  for (int i = 1 + flag_rm_dir; i < argc; ++i) {
    lstat(argv[i], &stat_file);
    if (S_ISDIR(stat_file.st_mode)) {
      rm_recursive(argv[i], flag_rm_dir);
    } else {
      rm_slink(argv[i]);
    }
  }
  return 0;
}