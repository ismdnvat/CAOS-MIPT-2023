#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/user.h>

typedef struct Counter {
    char filename[PATH_MAX];
    int counter;
    struct Counter* next;
} Counter;

typedef struct Counters {
    struct Counter* head;
} Counters;

void increment(Counters* counters, const char* filename, int value) {
    Counter* current = counters->head;
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            current->counter += value;
            return;
        }
        current = current->next;
    }

    Counter* new_entry = malloc(sizeof(Counter));
    new_entry->next = counters->head;
    new_entry->counter = value;
    strncpy(new_entry->filename, filename, PATH_MAX - 1);
    new_entry->filename[PATH_MAX - 1] = '\0';
    counters->head = new_entry;
}

void print(Counters* counters) {
    Counter* current = counters->head;
    while (current != NULL) {
        printf("%s:%d\n", current->filename, current->counter);
        current = current->next;
    }
}

void get_file_path(int fd, pid_t pid, char* path, size_t path_size) {
    char fd_path[PATH_MAX];
    snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd/%d", (int)pid, fd);
    ssize_t size = readlink(fd_path, path, path_size - 1);
    if(size < 0) {
        perror("readlink");
        path[0] = '\0';
        exit(EXIT_FAILURE);
    }
    path[size] = '\0';
<<<<<<< HEAD
}

int main(int argc, char *argv[]) {
    Counters counters;
    counters.head = NULL;
    int written = 1;
    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child_pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], &argv[1]);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(child_pid, &status, 0);

        while (WIFSTOPPED(status)) {
            ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
            waitpid(child_pid, &status, 0);
            struct user_regs_struct regs;
            ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
            if (regs.orig_rax == 1) {
                char filename[PATH_MAX];
                //char data[PATH_MAX];
                get_file_path(regs.rdi, child_pid, filename, PATH_MAX);
                //fprintf(stderr, "we search %s %lld\n", filename, regs.rdx);
                if (written == 1) {
                    increment(&counters, filename, regs.rdx);
                    written = 0;
                } else {
                    written = 1;
                }
            }
        }
        print(&counters);
        Counter* current = counters.head;
        while (current != NULL) {
            Counter* next = current->next;
            free(current);
            current = next;
        }
    }
    return 0;
=======
>>>>>>> 60ddfec1765a86682b84e1b169f3394d2d5ceb25
}

int main(int argc, char *argv[]) {
    Counters counters;
    counters.head = NULL;
    int written = 1;
    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child_pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], &argv[1]);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(child_pid, &status, 0);

        while (WIFSTOPPED(status)) {
            ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
            waitpid(child_pid, &status, 0);
            struct user_regs_struct regs;
            ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
            if (regs.orig_rax == 1) {
                char filename[PATH_MAX];
                //char data[PATH_MAX];
                get_file_path(regs.rdi, child_pid, filename, PATH_MAX);
                //fprintf(stderr, "we search %s %lld\n", filename, regs.rdx);
                if (written == 1) {
                    increment(&counters, filename, regs.rdx);
                    written = 0;
                } else {
                    written = 1;
                }
            }
        }
        print(&counters);
        Counter* current = counters.head;
        while (current != NULL) {
            Counter* next = current->next;
            free(current);
            current = next;
        }
    }
    return 0;
}