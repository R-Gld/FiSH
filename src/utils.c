#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <fcntl.h>
#include <unistd.h>

void manage_file_input(char *file_input) {
    if(file_input == NULL) return;
    int fd_in = open(file_input, O_RDONLY);
    if(fd_in < 0) {
        perror("open input file");
        exit(EXIT_FAILURE);
    }
    dup2(fd_in, STDIN_FILENO);
    close(fd_in);
}

void manage_file_output(char *file_output, bool file_output_append) {
    if(file_output == NULL) return;
    int flags = O_WRONLY | O_CREAT;
    flags |= (file_output_append ? O_APPEND : O_TRUNC);
    int fd_out = open(file_output, flags, 0644);
    if (fd_out < 0) {
        perror("open output file");
        exit(EXIT_FAILURE);
    }
    dup2(fd_out, STDOUT_FILENO);
    close(fd_out);
}