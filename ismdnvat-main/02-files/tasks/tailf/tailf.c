#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    perror("too little arguments");
    return EXIT_FAILURE;
  }
  const char *filename = argv[1];
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("cannot open file");
    exit(EXIT_FAILURE);
  }

  char file_content[1024];
  int read_bytes = read(fd, file_content, 1024);
  if (read_bytes > 0) {
    if(write(STDOUT_FILENO, file_content, read_bytes) != read_bytes) {
      perror("problem with write");
      exit(EXIT_FAILURE);
    };
  }

  if (read_bytes == -1) {
    close(fd);
    perror("cannot read file");
    exit(EXIT_FAILURE);
  }

  while (1) {
    read_bytes = read(fd, file_content, 1024);
    if (read_bytes > 0 && write(STDOUT_FILENO, file_content, read_bytes) != read_bytes) {
        perror("cannot write");
        exit(EXIT_FAILURE);
    } else if (read_bytes == -1) {
      close(fd);
      perror("cannot write");
      exit(EXIT_FAILURE);
    }
  }
  return EXIT_SUCCESS;
}

