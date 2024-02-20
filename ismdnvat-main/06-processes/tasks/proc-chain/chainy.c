#define _GNU_SOURCE
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>

enum {
    MAX_ARGS_COUNT = 256,
    MAX_CHAIN_LINKS_COUNT = 256
};

pid_t pid_arr[MAX_CHAIN_LINKS_COUNT];

typedef struct {
    char *command;
    uint64_t argc;
    char *argv[MAX_ARGS_COUNT];
} chain_link_t;

typedef struct {
    uint64_t chain_links_count;
    chain_link_t chain_links[MAX_CHAIN_LINKS_COUNT];
} chain_t;

void parse_commands(char *command, chain_link_t *link) {
    char *tmp = command;
    char *tmp2 = command;
    link->argc = 1;
    while ((tmp = strchr(tmp, ' '))) {
        link->argv[link->argc - 1] = strndup(tmp2, tmp - tmp2);
        link->argc++;
        tmp2 = ++tmp;
    }
    if (strlen(tmp2) > 0) {
      link->argv[link->argc - 1] = strdup(tmp2);
    }
    link->command = link->argv[0];
}

void create_chain(char *command, chain_t *chain) {
    char *tmp = command;
    char *tmp2 = command;
    chain->chain_links_count = 0;
    while ((tmp = strchr(tmp, '|'))) {
        if (tmp[0] == ' ') {
        ++tmp;
        }
        parse_commands(strndup(tmp2, tmp - tmp2 - 1), &(chain->chain_links[chain->chain_links_count]));
        tmp = tmp2 = tmp + 2;
        ++chain->chain_links_count;
    }
    if (strlen(tmp2) > 0) {
        if (tmp2[0] == ' ') {
            ++tmp2;
        }
        parse_commands(strdup(tmp2), &(chain->chain_links[chain->chain_links_count]));
        ++chain->chain_links_count;
    }
}

void run_chain(chain_t* chain) {
    int fd[2];
    int prev_pipe = -1;

  for (int i = 0; i < chain->chain_links_count; ++i) {
        if (i != chain->chain_links_count - 1 && pipe(fd) == -1) {
            perror("pipe error");
            exit(EXIT_FAILURE);
        }

        int pid = fork();
        if (pid < 0) {
            perror("fork error");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            if (i != 0) {
                dup2(prev_pipe, STDIN_FILENO);
                close(prev_pipe);
            }
            if (i != chain->chain_links_count - 1) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
            }
            execvp(chain->chain_links[i].command, chain->chain_links[i].argv);
            perror("execvp error");
            exit(EXIT_FAILURE);
        } else {
            if (prev_pipe != -1) {
                close(prev_pipe);
            }
            if (i != chain->chain_links_count - 1) {
                close(fd[1]);
                prev_pipe = fd[0];
            }
        }
    }

    for (size_t i = 0; i < chain->chain_links_count; ++i) {
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    fprintf(stderr, "%s\n", argv[1]);
    chain_t chain;
    create_chain(argv[1], &chain);
    run_chain(&chain);
    return 0;
}

