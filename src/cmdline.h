/*!
 * \file cmdline.h
 * \brief Header file for the command line parser.
 * \author Eric MERLET / Romain GALLAND
 * \version 2
 * The version 1 of this file was provided in the instructions.
 */

#ifndef CMDLINE_H
#define CMDLINE_H

#include <stddef.h>
#include <stdbool.h>

/*!
 * \def MAX_ARGS
 * \brief The maximum of arguments for a single command.
 */
#define MAX_ARGS 16

/*!
 * \def MAX_CMDS
 * \brief The maximum of commands for a single command line.
 */
#define MAX_CMDS 16

/*!
 * \struct cmd
 * \brief Structure representing a single command with its arguments.
 *
 * This structure holds the arguments of a single command. The arguments are stored as an array of
 * strings, with the last element being NULL. The number of arguments is stored in the "n_args" field.
 */
struct cmd {
  char *args[MAX_ARGS + 1];  /*!< Array of arguments for the command. */
  size_t n_args;             /*!< Number of arguments for the command. */
};


/*!
 * \struct line
 * \brief Structure representing a command line with multiple commands and redirections.
 *
 * This structure holds the commands and redirections of a single command line. The commands are stored
 * as an array of "cmd" structures, with the last element being NULL. The number of commands is stored
 * in the "n_cmds" field. The structure also holds the filenames for input and output redirections, as
 * well as a flag for background execution.
 */
struct line {
  struct cmd cmds[MAX_CMDS];  /*!< Array of commands in the command line. */
  size_t n_cmds;              /*!< Number of commands in the command line. */
  char *file_input;           /*!< Filename for input redirection. */
  char *file_output;          /*!< Filename for output redirection. */
  bool file_output_append;    /*!< Flag indicating whether to append to the output file. */
  bool background;            /*!< Flag indicating background execution. */
};

/*!
 * \fn static bool valid_cmdarg_filename(const char *word)
 * \brief Init the struct line given in parameter
 * All the bits of the structure are set to 0
 *
 * \param li pointer on the struct line to be initialized
 */
void line_init(struct line *li);


/*!
 * \fn int line_parse(struct line *li, const char *str)
 * \brief Parses the given string "str" and constructs the command line structure pointed to by "li".
 *
 * This function attempts to parse a command line contained in "str" and fills the structure "li"
 * accordingly. The command line may include commands separated by pipes "|", output redirections ">"
 * and ">>", input redirection "<", and background execution "&". Proper parsing updates the
 * structure "li" with commands, their arguments, and redirection or background information.
 *
 * The parsing process checks for various syntax errors like missing filenames after redirections,
 * invalid command or argument formats, improper use of pipes or redirections, and excess in the
 * number of commands or arguments as specified by MAX_CMDS and MAX_ARGS constants.
 *
 * \param li Pointer to the struct line where the parsed command line will be stored.
 * \param str Null-terminated string containing the command line to be parsed.
 * \return Returns 0 on successful parsing with a properly formed command line. <br>
 *         Returns -1 on failure, indicating a syntax error or invalid command line structure.
 *
 * \note This function uses dynamic memory allocation for storing individual arguments and filenames.
 *       It is the caller's responsibility to free these resources when no longer needed.
 */
int line_parse(struct line *li, const char *str);

/*!
 * Reset a struct line
 * 
 * Free dynamically allocated memory
 * All bytes occupied by the structure are set to 0
 * 
 * @param li pointer on the struct line to be reset
 */
void line_reset(struct line *li);

#endif
