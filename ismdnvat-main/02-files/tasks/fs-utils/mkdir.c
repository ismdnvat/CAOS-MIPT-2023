#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <getopt.h>
#include <linux/limits.h>
#include <errno.h>

typedef struct option option;

void my_mkdir(char* path, mode_t mode, int par_flg) {
  if (NULL == path) {
    perror("no arg");
    exit(EXIT_FAILURE);
  }
  if ((mkdir(path, mode)) == 0) {
    exit(EXIT_SUCCESS);
  } else if (!par_flg) {
    perror("no -p\n");
    exit(EXIT_FAILURE);
  }
  char* tmp_dub = NULL;
  char* ptr = path;
  for (; (ptr = strchr(ptr, '/')); ++ptr) {
//    printf("ptr = %s\n", ptr);
    tmp_dub = strndup(path, ptr - path);
//    printf("ending = %s\n", tmp_dub);
    mkdir(tmp_dub, mode);
  }
  if (mkdir(path, mode) == -1) {
    perror("error");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    perror("mkdir: missing operand");
    exit(EXIT_FAILURE);
  }
  int opt = 0;
  option long_options[] = {
    {"mode", required_argument,0,'m'},
    {0, 0, 0, 0}
  };
  mode_t mode = 0777;
  char* path = NULL;
  int par_flg = 0;
  optarg = "00000";
  while ((opt = getopt_long(argc, argv, "-:p:m:", long_options, NULL)) != -1) {
    printf("opt = %d, optopt=%c optarg = %s\n", opt, optopt, optarg);
    switch (opt) {
      case '?':
        perror("rm: missing argument");
        exit(EXIT_FAILURE);
      case 1:
        printf("case 1: %c\n", (char)opt);
        path = strdup(optarg);
        break;
      case 'p':
        printf("case p: %c\n", (char)opt);
        par_flg = 1;
        break;
      case 'm':
        printf("case m: %c\n", (char)opt);
        mode = strtol(optarg, NULL, 8);
        break;
      default:
        break;
    }
  }
  if (optind == argc) {
    if (path == NULL) {
      path = argv[optind - 1];
    } else {
      if (!strcmp(argv[optind - 1], "-p")) {
        par_flg = 1;
      }
    }
  }
  my_mkdir(path, mode, par_flg);
  return 0;
}
