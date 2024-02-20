#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>

static int signal_pipe[2];

static void handle_signal(int signo) {
    int result = write(signal_pipe[1], &signo, sizeof(signo));
    if (result != sizeof(signo)) {
        abort();
    }
}

int signalfd() {

    if (pipe(signal_pipe) == -1) {
        perror("Error creating signal pipe");
        exit(EXIT_FAILURE);
    }
    fcntl(signal_pipe[1], F_SETFL, fcntl(signal_pipe[1], F_GETFL, 0) | O_NONBLOCK);
    for (int signo = 1; signo < 32; ++signo) {
        struct sigaction sa;
        sa.sa_handler = handle_signal;
        sa.sa_flags = SA_RESTART;
        sigaction(signo, &sa, NULL);
    }
    //fprintf(stderr, "signal pipe = %d && %d\n", signal_pipe[0], signal_pipe[1]);
    return signal_pipe[0];
}

