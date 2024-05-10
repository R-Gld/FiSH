/**
 * @file fish.h
 * @brief Header file for the fish program.
 * @author Romain GALLAND
 * @version 1
 *
 * This file contains the definition of the structure representing a fish, as well as the prototypes of the functions
 */

#ifndef FISH_FISH_H
#define FISH_FISH_H

#include "cmdline.h"

#include <signal.h>

#include <stdbool.h>

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

/*!
 * \def BUFLEN
 * \brief Maximum length of the command line.
 * This constant defines the maximum length of the command line that can be entered by the user.
 */
#define BUFLEN 512

/*!
 * \def RESET
 * \brief Escape code to reset the color of the prompt.
 * This escape code is used to reset the color of the prompt after the command line.
 */
#define RESET "\x1B[0m"
/*!
 * \def GREEN
 * \brief Escape code for the color green.
 * This escape code is used to color the username and the current directory in green.
 */
#define GREEN "\x1B[32m"
/*!
 * \def YELLOW
 * \brief Escape code for the color yellow.
 * This escape code is used to color the FiSH shell name in yellow.
 */
#define YELLOW "\x1B[33m"
/*!
 * \def BLUE
 * \brief Escape code for the color blue.
 * This escape code is used to color the prompt symbol in blue.
 */
#define BLUE "\x1B[34m"
/*!
 * \def GRAY
 * \brief Escape code for the color gray.
 * This escape code is used to color the arrows in gray.
 */
#define GRAY "\x1B[90m"

/*!
 * \def YES_NO(i)
 * \brief Macro to convert a boolean value to a string.
 * This macro converts a boolean value to a string "Y" if the value is true, and "N" if the value is false.
 */
#define YES_NO(i) ((i) ? "Y" : "N")

/* All the docs are described in the file fish.c */

void execute_command_with_args(char *cmd, char *args[], struct sigaction *standard_sigint_action, struct line *li, struct piped *piped);
bool manage_intern_cmd(char *cmd, char *args[], struct line *li);
void cd(char *path);
void substitute_home(char *path, char *home);
void print_debug_line(struct line *li);
void sigchld_handler(int signum);
struct sigaction manage_sigaction();

#endif //FISH_FISH_H
