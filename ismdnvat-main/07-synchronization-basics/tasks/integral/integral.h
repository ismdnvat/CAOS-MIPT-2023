#pragma once

#include <stdint.h>
#include "wait.h"
#include <pthread.h>
#include <stdlib.h>
#include <error.h>
#include <stdio.h>
#include <errno.h>

typedef double field_t;

typedef field_t func_t(field_t);

typedef struct {
  field_t left_bound;
  field_t right_bound;
  func_t *func;
  int N;
  size_t thread_idx;
  void* integrator;
} integr_args_t;

typedef struct par_integrator {
  pthread_t *threads;
  func_t *func;
  size_t threads_num;
  field_t *global_res;
  integr_args_t *args_ptr;
} par_integrator_t;

void *trapezoidal_integral(void *args) {
  field_t current_res = 0.;
  integr_args_t my_args = *(integr_args_t *) args;
  field_t delta = (my_args.right_bound - my_args.left_bound) / my_args.N;
  current_res = (my_args.func(my_args.left_bound) + my_args.func(my_args.right_bound)) / 2.0;
  for (int i = 1; i < my_args.N; ++i) {
    field_t x_i = my_args.left_bound + i * delta;
    current_res += my_args.func(x_i);
  }
  current_res *= delta;
  par_integrator_t* tmp = (par_integrator_t*)my_args.integrator;
  tmp->global_res[my_args.thread_idx] = current_res;
  fflush(stderr);
  return NULL;
}

int par_integrator_init(par_integrator_t *self, size_t threads_num) {
  self->threads = malloc(threads_num * sizeof(pthread_t));
  self->global_res = malloc(threads_num * sizeof(field_t));
  self->args_ptr = malloc(threads_num * sizeof(integr_args_t));
  self->threads_num = threads_num;
  return 0;
}

int par_integrator_start_calc(par_integrator_t *self, func_t *func,
                              field_t left_bound, field_t right_bound) {
  self->func = func;
  int my_error = 0;
  for (size_t i = 0; i < self->threads_num; ++i) {
    self->args_ptr[i].left_bound = left_bound + i * (right_bound - left_bound) / self->threads_num;
    self->args_ptr[i].right_bound = left_bound + (i + 1) * (right_bound - left_bound) / self->threads_num;
    self->args_ptr[i].func = func;
    self->args_ptr[i].N = 10000 / self->threads_num;
    self->args_ptr[i].thread_idx = i;
    self->args_ptr[i].integrator = (void*) self;
    my_error = pthread_create(&(self->threads[i]), NULL, trapezoidal_integral, &(self->args_ptr[i]));
    if (my_error != 0) {
      perror("create");
      goto ERROR;
    }
  }
  ERROR:
  return my_error;
}

int par_integrator_get_result(par_integrator_t *self, field_t *result) {
  for (size_t i = 0; i < self->threads_num; ++i) {
    int my_error = pthread_join(self->threads[i], NULL);
    if (my_error != 0) {
      perror("join");
    }
  }
  *result = 0;
  for (int i = 0; i < self->threads_num; ++i) {
    *result += self->global_res[i];
  }
}

int par_integrator_destroy(par_integrator_t *self) {
  free(self->global_res);
  free(self->args_ptr);
  free(self->threads);
  return 0;
}