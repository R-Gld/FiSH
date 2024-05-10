/**
 * \file utils.h
 * \brief Header file for utility functions.
 * \author Romain GALLAND
 * \version 1
 */
#ifndef FISH_UTILS_H
#define FISH_UTILS_H

#include <stdbool.h>
#include "cmdline.h"

/*!
 * \def YES_NO(i)
 * \brief Macro to convert a boolean value to a string.
 * This macro converts a boolean value to a string "Y" if the value is true, and "N" if the value is false.
 */
#define YES_NO(i) ((i) ? "Y" : "N")


/**
 * \struct piped
 * \brief Structure helping manage piped commands.
 */
struct piped {
    struct cmd *previous; /*!< Pointer to the previous command. */
    struct cmd *next;     /*!< Pointer to the next command. */
    bool is_piped;        /*!< Flag indicating if the command is piped. */
};

/**
 * \fn piped_reset(struct piped *pip)
 * \brief Reset the structure to its initial state.
 * This function resets the structure to its initial state, by setting the pointers to NULL and the flag to false.
 * \param pip Pointer to the structure to reset.
 */
void piped_reset(struct piped *pip);


/**
 * \fn void manage_file_input(char *file_input)
 * \brief Redirects the input of the program to the file given as argument.
 *
 * \param file_input The name of the file to use as input.
 */
void manage_file_input(char *file_input);

/**
 * \fn void manage_file_output(char *file_output, bool file_output_append)
 * \brief Redirects the output of the program to the file given as argument.
 *
 * \param file_output The name of the file to use as output.
 * \param file_output_append bool indicating if the output is in mode TRUNC or APPEND.
 */
void manage_file_output(char *file_output, bool file_output_append);

/**
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

/**
 * \fn void substitute_home(char *path, char *home)
 * \brief Replace the home directory in the path by '~'.
 *
 * \param path The path to modify.
 * \param home The home directory. If NULL, the HOME environment variable is used.
 */
void substitute_home(char *path, char *home);


#endif //FISH_UTILS_H
