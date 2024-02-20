#include "lca.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct {
  int size_;
  int array_[1024];
} Array;

int FindIntersection(const Array* array_x, const Array* array_y) {
  size_t min_size = array_x->size_ > array_y->size_ ? array_y->size_ : array_x->size_;
  const Array* min_array = array_x->size_ > array_y->size_ ? array_y : array_x;
  for (int i = 1; i <= min_size; ++i) {
    if (array_x->array_[array_x->size_ - i] != array_y->array_[array_y->size_ - i]) {
//      printf("%d\n", min_array->array_[min_size - i + 1]);
      return min_array->array_[array_x->size_ - i + 1];
    }
  }
  return min_array->array_[0];
}

void FindAncestors(Array* array, pid_t pid) {
  char filename[512];
  sprintf(filename, "/proc/%d/stat", pid);
  int fd = open(filename, O_RDONLY);
  int trash1;
  char trash3;
  char trash2[256];
  int ppid;
  int c = read(fd, filename, sizeof(filename));
  sscanf(filename, "%d %s %c %d", &trash1, trash2, &trash3, &ppid);
  array->array_[array->size_++] = ppid;
  if (ppid > 1) {
    FindAncestors(array, ppid);
  }
  return;
}

pid_t find_lca(pid_t x, pid_t y) {
  Array array_x = {.size_ = 1};
  array_x.array_[0] = x;
  Array array_y = {.size_ = 1};
  array_y.array_[0] = y;
  FindAncestors(&array_x, x);
  FindAncestors(&array_y, y);
  return FindIntersection(&array_x, &array_y);
}