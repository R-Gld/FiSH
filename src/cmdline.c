/**
 * \file cmdline.c
 * \brief Source file for the command line parser.
 * \author Eric MERLET / Romain GALLAND
 * \version 2
 * The version 1 of this file was provided in the instructions.
 */

#include "cmdline.h"

#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


void line_init(struct line *li) {
  assert(li);
  memset(li, 0, sizeof(struct line));
}


/**
 * \fn static bool valid_cmdarg_filename(const char *word)
 * \brief Test the validity of command arguments or file names used in redirections
 * This function is static : it means that it is a local function, accessible only in this source file
 *
 * \param word pointer on the first char of string to test
 * \return true if the word is valid, false otherwise
 */
static bool valid_cmdarg_filename(const char *word){ 
   const char *forbidden = "<>&|";// forbidden characters in commands arguments and filenames

   size_t lenf = strlen(forbidden);
   for (size_t i = 0; i < lenf ; ++i){
      char *ptr = strchr(word, forbidden[i]);
      if (ptr) {
         return false;
      }
   }
   return true;
}

/**
 * \fn static void parse_error(const char *format, ...)
 * \brief Print the string "Error while parsing: ", followed by the string "format" to stderr
 * 
 * This function is static : it means that it is a local function, accessible only in this source file.
 * The string "format" may contain format specifications that specify how subsequent arguments are
 * converted for output. So, this function can be called with a variable number of parameters.
 * NB : vfprintf() uses the variable-length argument facilities of stdarg(3)
 * 
 * \param format string
 * \param ... variable number of parameters
 */
static void parse_error(const char *format, ...) {
  va_list ap;

  fprintf(stderr, "Error while parsing: ");
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
}

/**
 * \fn static int line_next_word(const char *str, size_t *index, char **pword)
 * \brief Search a new word in the string "str" from the "index" position
 * 
 * This function is static : it means that it is a local function, accessible only in this source file.
 * After the call, "index" contains the position of the last character used plus one.
 * If a word is found, it is copied to a dynamically allocated memory space. "pword" is a pointer
 * on a pointer which retrieves the address of this dynamically allocated memory space.
 * 
 * \param str pointer on the first char of the line entered by the user
 * \param index pointer on the index
 * \param pword pointer on a pointer which retrieves the address of this dynamically allocated memory space
 *
 * \return   0 if a word is found or if the end of the line is reached <br>
 *           -1 if a malformed line is detected <br>
 *           -2 if a memory allocation failure occurs
 */
static int line_next_word(const char *str, size_t *index, char **pword) {
  assert(str);
  assert(index);
  assert(pword);

  size_t i = *index;
  *pword = NULL;

  /* Eat space */
  while (str[i] != '\0' && isspace(str[i])) {
    ++i;
  }

  /* Check if it is the end of the line */
  if (str[i] == '\0') {
    *index = i;
    return 0;
  }

  size_t start = i;
  size_t end = i;
  if (str[i] == '"' || str[i] == '\'') {  // Handle both double quotes and single quotes (default can only handle double quotes)
    char quoteType = str[i];  // Save the quote type (' or ")
    ++start;
    do {
      ++i;
    } while (str[i] != '\0' && str[i] != quoteType);  // Match the same type of quote

    if (str[i] == '\0') {
      parse_error("Malformed line, unmatched %c\n", quoteType);
      return -1;
    }
    assert(str[i] == quoteType);
    end = i;
    ++i;
  }
  else {
    while (str[i] != '\0' && !isspace(str[i])) {
      ++i;
    }
    end = i;
  }

  *index = i;

  /* Copy this word */
  assert(end >= start);
  *pword = calloc(end - start + 1, sizeof(char));
  if (*pword == NULL) {
    fprintf(stderr, "Memory allocation failure\n");
    return -2;
  }
  memcpy(*pword, str + start, end - start);
  return 0;
}


/**
 * \fn int line_parse(struct line *li, const char *str)
 * \brief Parse the string "str" and construct the struct line pointed by "li"
 *
 * \param li the struct line to fill
 * \param str the string to parse
 * \return 0 on success, -1 on failure
 */
int line_parse(struct line *li, const char *str) {
  assert(li);
  assert(str);

  size_t len = strlen(str);
  assert(len >= 1);
  if (str[len -1] != '\n'){
    fprintf(stderr, "The command line is too long\n");
    char c = 0;
    while (c != '\n'){
      c = fgetc(stdin);
    }
    return -1;
  }

  size_t index = 0;
  size_t curr_n_cmd = 0;
  size_t curr_n_arg = 0;
  int valret = 0;

  for (;;) {
    /* get the next word */
    char *word;
    int err = line_next_word(str, &index, &word);
    if (err) {
      valret = -1;
      break;
    }

    if (!word) {
      break;
    }

#ifdef DEBUG
    fprintf(stderr, "\tnew word: \"%s\"\n", word);
#endif

    if (strcmp(word, "|") == 0) {
      free(word);

      if (li->background) {
        parse_error("No pipe allowed after a '&'\n");
        valret = -1;
        break;
      }

      if (li->file_output) {
        parse_error("No pipe allowed after an output redirection\n");
        valret = -1;
        break;
      }

      if (curr_n_arg == 0){
        parse_error("An empty command before a pipe detected\n");
        valret = -1;
        break;
      }

      li->cmds[curr_n_cmd].n_args = curr_n_arg;
      curr_n_arg = 0;
      ++curr_n_cmd;
    }
    else if (strcmp(word, ">") == 0 || strcmp(word, ">>") == 0) {
      bool append = strcmp(word, ">>") == 0;
      free(word);

      if (li->file_output) {
        parse_error("Output redirection already defined\n");
        valret = -1;
        break;
      }

      if (li->background) {
        parse_error("No output redirection allowed after a '&'\n");
        valret = -1;
        break;
      }

      err = line_next_word(str, &index, &word);
      if (err) {
        valret = -1;
        break;
      }

      if (!word) {
        parse_error("Waiting for a filename after an output redirection\n");
        valret = -1;
        break;
      }

      if (!valid_cmdarg_filename(word)){
        parse_error("Filename \"%s\" is not valid\n", word);
        free(word);
        valret = -1;
        break;
      }
      li->file_output = word;
      li->file_output_append = append;

    }
    else if (strcmp(word, "<") == 0) {
      free(word);

      if (li->file_input) {
        parse_error("Input redirection already defined\n");
        valret = -1;
        break;
      }

      if (li->background) {
        parse_error("No input redirection allowed after a '&'\n");
        valret = -1;
        break;
      }

      if (curr_n_cmd > 0){
        parse_error("Input redirection is only allowed for the first command\n");
        valret = -1;
        break;
      }

      err = line_next_word(str, &index, &word);
      if (err) {
        valret = -1;
        break;
      }

      if (!word) {
        parse_error("Waiting for a filename after an input redirection\n");
        valret = -1;
        break;
      }

      if (!valid_cmdarg_filename(word)){
        parse_error("Filename \"%s\" is not valid\n", word);
        free(word);
        valret = -1;
        break;
      }

      li->file_input = word;

    }
    else if (strcmp(word, "&") == 0) {
      free(word);

      if (li->background) {
        parse_error("More than one '&' detected\n");
        valret = -1;
        break;
      }

      if (curr_n_arg == 0){
        parse_error("An empty command before '&' detected\n");
        valret = -1;
        break;
      }

      li->background = true;
    }
    else {
      if (li->background) {
        free(word);
        parse_error("No more commands allowed after a '&'\n");
        valret = -1;
        break;
      }
      if (curr_n_cmd == MAX_CMDS) {
        free(word);
        parse_error("Too much commands. Max: %i\n", MAX_CMDS);
        valret = -1;
        break;
      }
      if (curr_n_arg == MAX_ARGS) {
        free(word);
        parse_error("Too much arguments. Max: %i\n", MAX_ARGS);
        valret = -1;
        break;
      }

      if (!valid_cmdarg_filename(word)){
        parse_error("Argument \"%s\" is not valid\n", word);
        free(word);
        valret = -1;
        break;
      }

      li->cmds[curr_n_cmd].args[curr_n_arg] = word;
      ++curr_n_arg;
    }
  } //end of the loop for

  if (!valret && curr_n_arg == 0) {
    if (curr_n_cmd > 0){
      parse_error("An empty command detected\n");
      valret = -1;
    }
    // in a real shell, "< fic" is equivalent to "test -r fic"
    else if (li->file_input){
      parse_error("Missing first command\n");
      valret = -1;
    }
    // in a real shell, "> fic" :
    // - creates the regular file "fic" if it does not exist,
    // - and truncates it if it already exists
    // in a real shell, ">> fic" :
    // - creates the regular file "fic" if it does not exist,
    // - and doesn't truncate it if it already exists
    else if (li->file_output){
      parse_error("Missing last command\n");
      valret = -1;
    }
  }

  if (curr_n_arg != 0) {
    li->cmds[curr_n_cmd].n_args = curr_n_arg;
    ++curr_n_cmd;
  }
  li->n_cmds = curr_n_cmd;
  return valret;
}

void line_reset(struct line *li) {
  assert(li);

  for (size_t i = 0; i < li->n_cmds; ++i) {
    for (size_t j = 0; j < li->cmds[i].n_args; ++j) {
      free(li->cmds[i].args[j]);
      li->cmds[i].args[j] = NULL; // useless here because of the call of memset()
    }
  }

  if (li->file_input) {
    free(li->file_input);
    li->file_input = NULL; // useless here because of the call of memset()
  }

  if (li->file_output) {
    free(li->file_output);
    li->file_output = NULL; // useless here because of the call of memset()
  }

  memset(li, 0, sizeof(struct line));
}
