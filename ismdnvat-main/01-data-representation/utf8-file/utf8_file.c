#include "utf8_file.h"
#include <string.h>

#define dbg() printf("sym{%d, '%s'}\n", sym.byte_size, sym.sym)

const size_t kPageSize = 4096;

typedef struct{
  uint8_t byte_size;
  char sym[8];
} utf8_sym;

utf8_file_t* utf8_fromfd(int fd) {
  utf8_file_t* utf8_file = malloc(sizeof(utf8_file_t));
  if (utf8_file == NULL) {
    return NULL;
  }
  utf8_file->fd = fd;
  return utf8_file;
}

// local nullptr indicator
int check_ptr2(const void * ptr1,const void * ptr2) {
  if (ptr1 == NULL || ptr2 == NULL) {
    errno = EINVAL;
    return 1;
  }
  return 0;
}

uint64_t scalar_repr(utf8_sym sym) {
  uint64_t res = 0;
  if (sym.byte_size == 1) {
    return sym.sym[0];
  }
  else{
    for (int i = sym.byte_size - 1; i >= 0; --i) {
      uint64_t tmp = 0;
      if (0 == i) {
        tmp = sym.sym[0] & (0x7F >> sym.byte_size);
      }
      else {
        tmp = sym.sym[i] & 0x3F;
      }
      res |= tmp << ((sym.byte_size - i - 1) * 6);
    }
  }
  return res;
}

utf8_sym scalar_to_utf(uint64_t scalar) {
  utf8_sym sym;
  size_t sz = 1;
  const char kIdMask = 0x80;
//  printf("scalar=%lX\n", scalar);

  for (uint64_t temp = scalar; temp > 0x3F; temp = temp >> 6, ++sz)
    ;
  sym.byte_size = sz;
  if (sz == 1) {
    sym.sym[0] = (char)(scalar & 0x7F);
  }
  else {
    for (int i = 1; i < sz; ++i) {
      sym.sym[i] = kIdMask | (scalar & 0x3F);
      scalar = scalar >> 6;
    }
    sym.sym[sz] = '\0';
    sym.sym[0] = (kIdMask >> (sz - 1)) | (scalar & 0x3F);
  }

  return sym;
}

utf8_sym overlong_trunc(utf8_sym sym) {
  return scalar_to_utf(scalar_repr(sym));
}

size_t get_size(const char inf_byte) {
  size_t res = 1;
  for (int i = 6; i > 0; --i) {
    if (((inf_byte >> i) & 1) == 0)
      break;
    else
      res += (inf_byte >> i) & 1;
  }
  if (res > 6) {
    errno = EINVAL;
    exit(-1);
  }
  return res;
}

utf8_sym read_sym(utf8_file_t* f) {
  utf8_sym sym;
  utf8_sym err_safe = {
      .byte_size = 0
  };

  if (read(f->fd, sym.sym, sizeof(char))) {
    sym.byte_size = get_size(sym.sym[0]);
//    printf("1st_sym =%d, ", sym.sym[0]);
    if (sym.byte_size > 1) {
      if(read(f->fd, sym.sym + 1, sym.byte_size - 1) == 0) {
        return err_safe;
      }
    }
  }
  else {
    return err_safe;
  }
  sym.sym[sym.byte_size] = '\0';
//  printf("read_sym = '%s', size=%d\n", sym.sym, sym.byte_size);
  return sym;
}

int utf8_read(utf8_file_t* f, uint32_t* res, size_t count) {
  res[0] = 0;
  if(check_ptr2(f, res))
    return 1;
  size_t sym_cnt = 0;
  size_t offs = 0;
  for (;sym_cnt < count;++sym_cnt) {
    utf8_sym sym = read_sym(f);
//    printf("sym_cnt =%ld, sym={%d, '%s'}\n", sym_cnt, sym.byte_size, sym.sym);
    if (0 == sym.byte_size) {
      break;
    }else{
      sym = overlong_trunc(sym);
//      printf("[overlonged] offs=%ld sym_cnt =%ld, sym={%d, '%s'}\n", offs, sym_cnt, sym.byte_size, sym.sym);
      memcpy((char *)res + offs, sym.sym, sym.byte_size);
      offs += sym.byte_size;
    }
  }
//  printf("'%s' len=%ld\n", (char *)res, offs);
  return offs;
}

int utf8_write(utf8_file_t* f, const uint32_t* str, size_t count){
  if(check_ptr2(f, str))
    return 1;

  return write(f->fd, str, count);
}
