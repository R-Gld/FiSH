/*!
 * \file utils.c
 * \brief Implementation of utility functions.
 * \author Romain GALLAND
 * \version 1
 */

/*!
 * \def _GNU_SOURCE
 * \brief Define to enable the use of some GNU extensions.
 *
 * Used for the function asprintf.
 */
#define _GNU_SOURCE

#include "utils.h"

#include "cmdline.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


void init_pipe_control(struct pipe_control *pc) {
    pc->pipe_prev[PREAD] = -1;
    pc->pipe_prev[PWRITE] = -1;
    pc->pipe_next[PREAD] = -1;
    pc->pipe_next[PWRITE] = -1;
}

void close_pipe(int pipe[2]) {
    if (pipe[PREAD] != -1 && close(pipe[PREAD]) == -1) { perror("close pipe[READ]"); exit(EXIT_FAILURE); }
    if (pipe[PWRITE] != -1 && close(pipe[PWRITE]) == -1) { perror("close pipe[WRITE]"); exit(EXIT_FAILURE); }
}


void manage_file_input(char *file_input) {
    if(file_input == NULL) return;
    int fd_in = open(file_input, O_RDONLY);
    if(fd_in < 0) {
        char *msg;
        asprintf(&msg, "open input file '%s'", file_input);
        perror(msg);
        free(msg);
        exit(EXIT_FAILURE);
    }
    if(dup2(fd_in, STDIN_FILENO) == -1) { perror("dup2 fd_in"); exit(EXIT_FAILURE); }
    if(close(fd_in) == -1) { perror("close fd_in"); exit(EXIT_FAILURE); }
}

void manage_file_output(char *file_output, bool file_output_append) {
    if(file_output == NULL) return;
    int flags = O_WRONLY | O_CREAT;
    flags |= (file_output_append ? O_APPEND : O_TRUNC);
    int fd_out = open(file_output, flags, 0644);
    if (fd_out < 0) {
        char *msg;
        asprintf(&msg, "open output file '%s'", file_output);
        perror(msg);
        free(msg);
        exit(EXIT_FAILURE);
    }
    if(dup2(fd_out, STDOUT_FILENO) == -1) { perror("dup2 fd_out"); exit(EXIT_FAILURE); }
    if(close(fd_out) == -1) { perror("close fd_out"); exit(EXIT_FAILURE); }
}


void print_debug_line(struct line *li) {
    fprintf(stderr, "Command line:\n");
    fprintf(stderr, "\tNumber of commands: %zu\n", li->n_cmds);

    for (size_t i = 0; i < li->n_cmds; ++i) {
        fprintf(stderr, "\t\tCommand #%zu:\n", i);
        fprintf(stderr, "\t\t\tNumber of args: %zu\n", li->cmds[i].n_args);
        fprintf(stderr, "\t\t\tArgs:");
        for (size_t j = 0; j < li->cmds[i].n_args; ++j) {
            fprintf(stderr, " \"%s\"", li->cmds[i].args[j]);
        }
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "\tRedirection of input: %s\n", YES_NO(li->file_input));
    if (li->file_input) {
        fprintf(stderr, "\t\tFilename: '%s'\n", li->file_input);
    }

    fprintf(stderr, "\tRedirection of output: %s\n", YES_NO(li->file_output));
    if (li->file_output) {
        fprintf(stderr, "\t\tFilename: '%s'\n", li->file_output);
        fprintf(stderr, "\t\tMode: %s\n", li->file_output_append ? "APPEND" : "TRUNC");
    }

    fprintf(stderr, "\tBackground: %s\n", YES_NO(li->background));
}


void substitute_home(char *path, char *home) {
    if(home == NULL || home[0] == '\0') {
        home = getenv("HOME");
    }
    if (home == NULL) {
        return;
    }
    if (strncmp(path, home, strlen(home)) == 0) {
        char *new_path = malloc(strlen(path) + 1);
        if (new_path == NULL) {
            perror("malloc");
            return;
        }
        new_path[0] = '~';
        strcpy(new_path + 1, path + strlen(home));
        strcpy(path, new_path);
        free(new_path);
    }
}

void init_background_data(volatile struct bg_data background_data) {
    for (size_t i = 0; i < BG_MAX_SIZE; ++i) {
        background_data.bg_array[i] = -1;
    }
    background_data.bg_array_size = 0;

    for (size_t i = 0; i < BG_MAX_SIZE; ++i) {
        init_exit_status(&background_data.exit_statuses[i]);
    }
    background_data.exit_statuses_size = 0;
}

void init_exit_status(volatile struct background_exit_status *exit_status) {
    exit_status->pid = -1;
    exit_status->status_data = -1;
    exit_status->signaled = -1;
}