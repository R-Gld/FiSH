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
#include "utils.h"

#include <signal.h>

#include <stdbool.h>

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
 * \def RED
 * \brief Escape code for the color red.
 * This escape code is used to color the error messages in red.
 */
#define RED "\x1B[31m"

/*!
 * \def BOLD
 * \brief Escape code for the bold style.
 * This escape code is used to make the prompt symbol bold.
 */
#define BOLD "\x1B[1m"

/*!
 * \def ITALIC
 * \brief Escape code for the italic style.
 * This escape code is used to make the prompt symbol italic.
 */
#define ITALIC "\x1B[3m"



/* All the docs are described in the file fish.c */

void execute_command_with_args(char *cmd, char *args[], struct sigaction *standardSigintAction, struct line *line, struct pipe_control *pipeControl, size_t cmd_index, int *exit_code);
bool manage_intern_cmd(char *cmd, char *args[], struct line *li);
void cd(char *path);
void substitute_home(char *path, char *home);
void sigchld_handler(int signum);
struct sigaction manage_sigaction();
void apply_ignore(int signal, struct sigaction *old_sigaction);

#endif //FISH_FISH_H
