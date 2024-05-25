/*!
 * \file utils.h
 * \brief Header file for utility functions.
 * \author Romain GALLAND
 * \version 1
 */
#ifndef FISH_UTILS_H
#define FISH_UTILS_H

#include <stdbool.h>
#include <sys/types.h>
#include "cmdline.h"

/*!
 * \def PREAD
 * \brief Index of the read end of the pipe.
 */
#define PREAD 0
/*!
 * \def PWRITE
 * \brief Index of the write end of the pipe.
 */
#define PWRITE 1

/*!
 * \def YES_NO(i)
 * \brief Macro to convert a boolean value to a string.
 * This macro converts a boolean value to a string "Y" if the value is true, and "N" if the value is false.
 */
#define YES_NO(i) ((i) ? "Y" : "N")

/*!
 * \def BG_MAX_SIZE
 * \brief Size of the array containing the pids of the commands executed in background
 */
#define BG_MAX_SIZE 1024


/*!
 * \struct pipe_control
 * \brief Structure helping manage pipes.
 */
struct pipe_control {
    int pipe_prev[2]; /*!< File descriptors for the previous pipe. */
    int pipe_next[2]; /*!< File descriptors for the next pipe. */
};

/*!
 * \struct background_exit_status
 * \brief Structure representing the exit status of a background process.
 * This structure holds the PID of the process, as well as the status of the process when it exited.
 * if signaled is 1, the process was killed by a signal. Otherwise it exited normally.
 * then status_data is the signal number if signaled is 1, or the exit status if signaled is 0.
 */
struct background_exit_status {
    /*!
     * \var pid_t pid
     * \brief The PID of the process.
     */
    volatile pid_t pid;
    /*!
     * \var int signaled
     * \brief If signaled is 1, the process was killed by a signal. Otherwise it exited normally.
     */
    volatile int signaled;
    /*!
     * \var int status_data
     * \brief If signaled is 1, the process was killed, so status_data is the signal number. Otherwise it is the exit status.
     */
    volatile int status_data;
};

/*!
 * \struct bg_data
 * \brief Structure holding the background processes.
 */
struct bg_data {
    /*!
     * \var pid_t bg_array[BG_MAX_SIZE]
     * \brief Array of background processes.
     */
    volatile pid_t bg_array[BG_MAX_SIZE];

    /*!
     * \var size_t bg_array_size
     * \brief Size of the array of background processes.
     */
    volatile size_t bg_array_size;

    /*!
     * \var struct background_exit_status exit_statuses[BG_MAX_SIZE]
     * \brief Array of
     */
    volatile struct background_exit_status exit_statuses[BG_MAX_SIZE];

    volatile size_t exit_statuses_size;
};

/*!
 * \fn void init_pipe_control(struct pipe_control *pc)
 * \param pc The pipe_control structure to initialize.
 */
void init_pipe_control(struct pipe_control *pc);

/*!
 * \fn void close_pipe(int pipe[2])
 * \param pipe The pipe to close.
 */
void close_pipe(int pipe[2]);

/*!
 * \fn void manage_file_input(char *file_input)
 * \brief Redirects the input of the program to the file given as argument.
 *
 * \param file_input The name of the file to use as input.
 */
void manage_file_input(char *file_input);

/*!
 * \fn void manage_file_output(char *file_output, bool file_output_append)
 * \brief Redirects the output of the program to the file given as argument.
 *
 * \param file_output The name of the file to use as output.
 * \param file_output_append bool indicating if the output is in mode TRUNC or APPEND.
 */
void manage_file_output(char *file_output, bool file_output_append);

/*!
 * \fn void print_debug_line(struct line *li)
 * \brief Print the content of a line structure for debug purposes.
 *
 * Prints the following information:<br>
 * - Number of commands<br>
 * - For each command:<br>
 *  - Number of arguments<br>
 *  - Arguments<br>
 * - Redirection of input<br>
 *  - Filename for input redirection if any<br>
 * - Redirection of output<br>
 *  - Filename for output redirection if any<br>
 *  - Mode of output redirection (APPEND or TRUNC)<br>
 * - Background execution<br>
 *
 * \param li The line structure to print.
 */
void print_debug_line(struct line *li);

/*!
 * \fn void substitute_home(char *path, char *home)
 * \brief Replace the home directory in the path by '~'.
 *
 * \param path The path to modify.
 * \param home The home directory. If NULL, the HOME environment variable is used.
 */
void substitute_home(char *path, char *home);

/*!
 * \fn void init_background_data(pid_t *bg_array)
 * \brief Initialize the array of background processes.
 * \param bg_array The array to initialize.
 *
 * Set `-1` in all the cells of the array.
 */
void init_background_data(volatile struct bg_data background_data);

/*!
 * \fn void print_backgrounds_processes(pid_t *bg_array)
 * \param exit_status the background_exit_status struct to initialize.
 */
void init_exit_status(volatile struct background_exit_status *exit_status);


#endif //FISH_UTILS_H
