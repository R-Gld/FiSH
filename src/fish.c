/**
 * \file fish.c
 * \brief Main file of the FiSH shell.
 * \author Romain GALLAND / Eric MERLET
 * \version 2
 *
 * This file contains the main function of the FiSH shell.
 * and the main "algorithm" of the shell.
 *
 * The version 1 of this file was provided in the instructions.
 *
 */

/*!
 * \def _DEFAULT_SOURCE
 * \brief Define to enable the use of some GNU extensions.
 * This define is used to enable the use of some GNU extensions in the code.
 */
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>


#include "fish.h"
#include "cmdline.h"
#include "utils.h"

/**
 * \var bool debug
 * \brief Flag to enable debug messages.
 */
volatile bool debug = false;


/**
 * \fn int main()
 * \brief Main function of the FiSH shell.
 * This function is the main loop of the shell. It reads the command line entered by the user,
 * parses it, and executes the commands.
 * The shell supports the following internal commands:
 * - exit: exit the shell
 * - cd: change the current working directory
 * The shell also supports the following redirections:
 * - input redirection (<)
 * - output redirection (>)
 * - output redirection in append mode (>>)
 *
 * \return  0 if the program ends correctly, <br>
 *          1 otherwise
 */
int main() {
    printf(YELLOW "\n       _______ _________ _______          \n      (  ____ \\\\__   __/(  ____ \\|\\     /|\n      | (    \\/   ) (   | (    \\/| )   ( |\n      | (__       | |   | (_____ | (___) |\n      |  __)      | |   (_____  )|  ___  |\n      | (         | |         ) || (   ) |\n      | )      ___) (___/\\____) || )   ( |\n      |/       \\_______/\\_______)|/     \\|\n\n\n" RESET);

    struct line li;
    char buf[BUFLEN];
    line_init(&li);

    struct sigaction sa_standard_SIGINT = manage_sigaction();

    struct passwd *user_data = getpwuid(getuid());
    char *home = getenv("HOME");
    if (home == NULL) /* -> */ home = user_data->pw_dir;

    char *username = user_data->pw_name;

    for (;;) {
        char *current_dir = getcwd(NULL, 0);
        if(current_dir == NULL) { perror("getcwd (current_dir)"); exit(EXIT_FAILURE); }
        substitute_home(current_dir, home);

        printf(YELLOW "FiSH " GRAY "➔" GREEN " %s " GRAY "➔" BLUE " %s" RESET "\n\t➔ ", username, current_dir);
        free(current_dir);
        fgets(buf, BUFLEN, stdin);

        int err = line_parse(&li, buf);
        if (err) {
            //the command line entered by the user isn't valid
            line_reset(&li);
            continue;
        }

        if(debug) print_debug_line(&li);

        size_t number_of_cmds = li.n_cmds;
        struct piped piped;

        // Exécute chaque commande détectée avec ses arguments
        for (size_t i = 0; i < number_of_cmds; i++) {
            if (li.cmds[i].n_args > 0) {
                piped_reset(&piped);
                piped.next       =    (i < number_of_cmds - 1 ? &(li.cmds[i+1]) : NULL);
                piped.previous   =    (i > 0 ? &(li.cmds[i-1]) : NULL);
                piped.is_piped   =    (number_of_cmds > 1);

                execute_command_with_args(li.cmds[i].args[0], li.cmds[i].args, &sa_standard_SIGINT, &li, &piped);
                piped_reset(&piped);
            }
        }

        line_reset(&li);
    }
}


/**
 * \fn void execute_command_with_args(char *cmd, char *args[], struct sigaction *standard_sigint_action, struct line *li)
 * \brief Execute a command with its arguments.
 *
 * This function executes the command given in argument with its arguments.
 * It also handles the input and output redirections, the background execution, and the internal commands.
 *
 *
 * \param cmd The command to execute.
 * \param args The arguments of the command.
 * \param standard_sigint_action The action to execute when the SIGINT signal is received.
 * \param li The line structure of the command executed.
 * \param piped The piped structure of the command executed.
 */
void execute_command_with_args(
            char *cmd,
            char *args[],
            struct sigaction *standard_sigint_action,
            struct line *li,
            struct piped *piped
        ) {

    if(debug) {
        fprintf(stderr, "\n\n\tCommand: %s\n", cmd);
        if(piped->next != NULL) {
            fprintf(stderr, "\t\tPiped next: %s\n", piped->next->args[0]);
        }
        if(piped->previous != NULL) {
            fprintf(stderr, "\t\tPiped previous: %s\n", piped->previous->args[0]);
        }
        fprintf(stderr, "\t\tPiped: %s\n", YES_NO(piped->is_piped));
    }


    if (manage_intern_cmd(cmd, args, li)) return;

    bool background = li->background;

    if(!background) { // If the command is not executed in background, the SIGINT signal is 'un-ignored'. The previous action was ignore and its action is saved in ign_sa
        if(sigaction(SIGINT, standard_sigint_action, NULL) == -1) {
            perror("sigaction background");
            exit(EXIT_FAILURE);
        }
    }
    pid_t pid = fork();

    if(debug) fprintf(stderr, "\tpid created %d\n", pid);

    if (pid == 0) { // Child process
        char *file_input = li->file_input;

        if(background && file_input == NULL) {
            file_input = "/dev/null";
        }

        manage_file_input(file_input);

        manage_file_output(li->file_output, li->file_output_append);

        // Execute the command with its arguments
        if (execvp(cmd, args) == -1) {
            if(errno == ENOENT) {
                fprintf(stderr, "%s: Command not found\n", cmd);
            } else {
                perror("execvp");
            }
            exit(102);
        }
    }
    else if(pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else { // Parent process
        apply_ignore(SIGINT, NULL);

        if (background) {
            printf(" BG: Command `%d` running in background\n", pid);
        } else {
            int status;
            waitpid(pid, &status, 0); // Attend que l'enfant termine
            if (WIFEXITED(status) && WEXITSTATUS(status) != 102) {
                fprintf(stderr, " FG: Command `%d` exited with status %d\n", pid, WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status)) {
                fprintf(stderr, " FG: Command `%d` killed by signal %d\n", pid, WTERMSIG(status));
            }
        }
    }
}


/**
 * \fn bool manage_intern_cmd(char *cmd, char *args[], struct line *li)
 * \brief Manage the internal commands of the shell.
 *
 * The internals commands are the following:
 * - exit: exit the shell
 * - cd: change the current working directory
 *
 * \param cmd the command to manage
 * \param args the arguments of the command
 * \param li the line structure of the command executed (used to line_reset (free) the structure if the command is exit)
 * \return bool true if the command is an internal command, false otherwise
 */
bool manage_intern_cmd(char *cmd, char *args[], struct line *li) {
    if(strcmp(cmd, "exit") == 0) {
        int exit_n = 0;
        if(args[1] != NULL) {
            exit_n = atoi(args[1]);
            if(args[2] != NULL) {
                fprintf(stderr, "exit: too many arguments\n");
                return true;
            }
        }
        line_reset(li);
        exit(exit_n);
    }

    if(strcmp(cmd, "cd") == 0) {
        if(args[2] != NULL) {
            fprintf(stderr, "cd: too many arguments\n");
            return true;
        }
        cd(args[1]);
        return true;
    }

    if(strcmp(cmd, "debug") == 0) {
        if(args[2] != NULL) {
            fprintf(stderr, "debug: too many arguments\n");
            return true;
        }
        debug = !debug;
        fprintf(stderr, "Debug mode %s\n", YES_NO(debug));
        return true;
    }
    return false;
}

/**
 * \fn void cd(char *path)
 * \brief Change the current working directory.
 * Can handle '~' as a shortcut for the HOME directory.
 *
 * \param path The new working directory. If NULL, the HOME directory is used.
 */
void cd(char *path) {
    char *resolvedPath = NULL;
    char *homePath = getenv("HOME");

    if (path == NULL) {
        path = homePath;
    } else
    if (path[0] == '~') {
        if (path[1] == '\0') {
            path = homePath;
        }
        else if (path[1] == '/') {
            resolvedPath = malloc(strlen(homePath) + strlen(path));
            if (resolvedPath == NULL) {
                perror("malloc");
                return;
            }
            sprintf(resolvedPath, "%s%s", homePath, path + 1);
            path = resolvedPath;
        } else {
            fprintf(stderr, "cd: no such file or directory: %s\n", path);
            return;
        }
    }

    if (chdir(path) == -1) {
        perror("chdir");
    }
}

/**
 * \fn void sigchld_handler(int signum)
 * \brief Handler for the SIGCHLD signal.
 * This handler is called when a child process terminates.
 *
 * It prints a message to the standard error output indicating the termination of the child process.
 *
 * Example:<br>
 *  <ul>
 *  <li>" BG: Command `1234` exited with status 0"</li>
 *  <li>" BG: Command `1235` killed by signal 9"</li>
 *  </ul>
 *
 * \param signum The signal number. (Not used)
 */
void sigchld_handler(int signum) {
    int status;
    pid_t pid;
    char buffer[128];

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            int n = snprintf(buffer, sizeof(buffer), " BG: Command `%d` exited with status %d\n", pid, WEXITSTATUS(status));
            if (n > 0) {
                write(STDERR_FILENO, buffer, n);
            }
        }
        else if (WIFSIGNALED(status)) {
            int n = snprintf(buffer, sizeof(buffer), " BG: Command `%d` killed by signal %d\n", pid, WTERMSIG(status));
            if (n > 0) {
                write(STDERR_FILENO, buffer, n);
            }
        }
    }
}

/**
 * \fn void manage_sigaction()
 * \brief Manage the signal actions.
 *
 * This function manages the signal actions for the shell.
 * It ignores the SIGINT signal and sets the handler for the SIGCHLD signal.
 *
 * \return The previous action for the SIGINT signal.
 */
struct sigaction manage_sigaction() {
    struct sigaction sa_standard_SIGINT;
    apply_ignore(SIGINT, &sa_standard_SIGINT);

    struct sigaction sa_SIGCHILD;
    sigemptyset(&sa_SIGCHILD.sa_mask);
    sa_SIGCHILD.sa_flags = SA_RESTART;
    sa_SIGCHILD.sa_handler = sigchld_handler;
    if(sigaction(SIGCHLD, &sa_SIGCHILD, NULL) == -1) { perror("sigaction"); exit(EXIT_FAILURE); }
    return sa_standard_SIGINT;
}

/**
 * \fn void apply_ignore(int signal, struct sigaction *old_sigaction)
 * \TODO doc
 * \param signal
 * \param old_sigaction
 */
void apply_ignore(int signal, struct sigaction *old_sigaction) {
    struct sigaction sa_ignore;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = SA_RESTART;
    sa_ignore.sa_handler = SIG_IGN;
    if(sigaction(signal, &sa_ignore, old_sigaction) == -1) { perror("sigaction"); exit(EXIT_FAILURE); }
}
