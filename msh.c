#define _POSIX_C_SOURCE 200809l

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //task2  added
#include <unistd.h>//task3 added
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h> //task4 added
#include <signal.h> //task6 added
#include <fcntl.h>// Task7 added
#include <poll.h>

#define MAX_BACKGROUND_JOBS 256

// TASK 6: Keep track of background child processes
                 static pid_t background_jobs[MAX_BACKGROUND_JOBS];
                 static size_t background_count = 0;

// TASK 6: Recap  This prevents completed background children from becoming zombies.
static int reap_background_jobs(void)
{
                              size_t i = 0;
    int completed = 0;

                                while (i < background_count)
    {
        int status;
        pid_t result = waitpid(background_jobs[i], &status, WNOHANG);

        if (result == background_jobs[i])
        {
            printf("\n[msh] Background process %ld finished",
                   (long)background_jobs[i]);

            if (WIFEXITED(status))
            {
                printf(" with status %d", WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
                printf(" after signal %d", WTERMSIG(status));
            }

            printf("\n");

            // Remove the completed process from the list
            background_jobs[i] = background_jobs[background_count - 1];
            background_count--;
            completed++;
        }
        else if (result == -1 && errno == ECHILD)
        {
            // The process has already been collected
            background_jobs[i] = background_jobs[background_count - 1];
            background_count--;
        }
        else
        {
            i++;
        }
    }

    return completed;
}

                     // TASK 6: Wait for input while periodically checking background jobs
static int wait_for_input(int last_status)
{
    struct pollfd input_fd;

    input_fd.fd = STDIN_FILENO;
    input_fd.events = POLLIN;
    input_fd.revents = 0;

    while (1)
    {
        int result = poll(&input_fd, 1, 250);

        if (result > 0)
        {
            return 0;
        }

        if (result == 0)
        {
            // Check for completed background processes
            if (reap_background_jobs() > 0)
            {
                printf("msh[%d]> ", last_status);
                fflush(stdout);
            }

            continue;
        }

        if (errno == EINTR)
        {
            continue;
        }

        perror("poll");
        return -1;
    }
}

// TASK 5 and TASK 6: Wait for a foreground process while reaping

static int wait_for_foreground(pid_t pid, int *status)
{
    while (1)
    {
        pid_t result = waitpid(pid, status, WNOHANG);

        if (result == pid)
        {
            return 0;
        }

        if (result == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("waitpid");
            return -1;
        }

        
   reap_background_jobs();

        // Wait briefly before checking again.
        poll(NULL, 0, 50);
    }
}

// TASK 2: Split the command line into arguments.
static int split_line(char *line, char ***args_out, size_t *argc_out)
{
    size_t capacity = 8;
    size_t argc = 0;

    char **args = malloc(capacity * sizeof(char *));

    if (args == NULL)
    {
        perror("malloc");
        return -1;
    }

    char *token = strtok(line, " \t\r\n");

    while (token != NULL)
    {
        if (argc + 1 >= capacity)
        {
            capacity *= 2;

            char **new_args = realloc(args,
                                      capacity * sizeof(char *));

            if (new_args == NULL)
            {
                perror("realloc");
                free(args);
                return -1;
            }

            args = new_args;
        }

        args[argc++] = token;
        token = strtok(NULL, " \t\r\n");
    }

    // TASK 2: NULL terminate the argument list
    args[argc] = NULL;

    *args_out = args;
    *argc_out = argc;

    return 0;
}

// TASK 4: Display built-in command help
static void display_help(void)
{
    printf("Available msh commands:\n");
    printf("  cd [directory]  Change the current directory\n");
    printf("  help            Display this help message\n");
    printf("  exit            Exit the shell\n");
    printf("\n");
    printf("External commands are also supported.\n");
    printf("Use command & to run a command in the background.\n");
    printf("Use command > file to redirect output to a file.\n");
}

// TASK 5: Convert the child process result into a shell status
static int get_exit_status(int status)
{
    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }

    if (WIFSIGNALED(status))
    {
        return 128 + WTERMSIG(status);
    }

    return 1;
}

int main(void)
{
   // TASK 1: getline() manages the input line dynamically
    char *line = NULL;
    size_t line_capacity = 0;
    ssize_t nread;

  // Store the previous command's exit status
    int last_status = 0;

    // TASK 8: The shell ignores Ctrl+C. default SIGINT behaviour.
    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
    {
        perror("signal");
        return EXIT_FAILURE;
    }

    while (1)
    {
        // TASK 6: Reap completed background jobs
        reap_background_jobs();

         printf("msh[%d]> ", last_status);
        fflush(stdout);

 
        if (wait_for_input(last_status) == -1)
        {
            break;
        }

      // Read a line of any length
        nread = getline(&line, &line_capacity, stdin);

        // TASK 1: Handle Ctrl-D / end-of-file
        if (nread == -1)
        {
            if (feof(stdin))
            {
                printf("\nExiting msh...\n");
                break;
            }

            perror("getline");
            clearerr(stdin);
            last_status = 1;
            continue;
        }
		
        char **args = NULL;
        size_t argc = 0;
	
        if (split_line(line, &args, &argc) == -1)
        {
            last_status = 1;
            continue;
        }

        // TASK 2: Ignore blank lines
        if (argc == 0)
        {
            free(args);
            continue;
        }

        // TASK 6: Detect a trailing background symbol (&)
        int background = 0;

        if (strcmp(args[argc - 1], "&") == 0)
        {
            background = 1;
            args[--argc] = NULL;
        }

        if (argc == 0)
        {
            free(args);
            continue;
        }

          // TASK 4: BUILT-IN COMMANDS: help
        if (strcmp(args[0], "help") == 0)
        {
            if (background)
            {
                fprintf(stderr,
                        "msh: built-in commands cannot run in background\n");
                last_status = 1;
            }
            else
            {
                display_help();
                last_status = 0;
            }

            free(args);
            continue;
        }

        if (strcmp(args[0], "exit") == 0)
        {
            free(args);
            break;
        }

        
        if (strcmp(args[0], "cd") == 0)
        {
            if (background)
            {
                fprintf(stderr,
                        "msh: cd cannot run in background\n");
                last_status = 1;
            }
            else
            {
                char *directory;

                if (args[1] == NULL)
                {
                    // cd without an argument goes to HOME
                    directory = getenv("HOME");
                }
                else
                {
                    directory = args[1];
                }

                if (directory == NULL || chdir(directory) == -1)
                {
                    perror("cd");
                    last_status = 1;
                }
                else
                {
                    last_status = 0;
                }
            }

            free(args);
            continue;
        }

        
        // TASK 7: OUTPUT REDIRECTION
        

        char *output_file = NULL;
        int redirection_error = 0;

        for (size_t i = 0; i < argc; i++)
        {
            if (strcmp(args[i], ">") == 0)
            {
                // Require exactly one filename after >
                if (output_file != NULL ||
                    i + 1 >= argc ||
                    i + 2 != argc)
                {
                    fprintf(stderr,
                            "msh: invalid output redirection\n");
                    redirection_error = 1;
                    break;
                }

                output_file = args[i + 1];

                // Remove > and filename from command arguments
                args[i] = NULL;
                argc = i;
                break;
            }
        }

        if (redirection_error || argc == 0)
        {
            if (redirection_error)
            {
                last_status = 2;
            }

            free(args);
            continue;
        }

        
        // TASK 3: CREATE A CHILD PROCESS
        

        pid_t pid = fork();

        if (pid == -1)
        {
            // TASK 3: Report fork failure
            perror("fork");
            last_status = 1;
            free(args);
            continue;
        }

        if (pid == 0)
        {
        
            // TASK 8: Restore normal Ctrl+C behaviour
        
            signal(SIGINT, SIG_DFL);

        
            // TASK 7: Redirect output in the child
        
            if (output_file != NULL)
            {
                int fd = open(output_file,
                              O_WRONLY | O_CREAT | O_TRUNC,
                              0644);

                if (fd == -1)
                {
                    perror("open");
                    _exit(126);
                }

                if (dup2(fd, STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    close(fd);
                    _exit(126);
                }

                close(fd);
            }

                    // TASK 3: Execute external command
        

            execvp(args[0], args);

            // Reached only when execvp fails
            int exec_error = errno;
            perror(args[0]);

            if (exec_error == ENOENT)
            {
                _exit(127);
            }

            _exit(126);
        }
        else
        {
                   // TASK 6: BACKGROUND EXECUTION
        
            if (background)
            {
                printf("Started background process with PID %ld\n",
                       (long)pid);

                last_status = 0;

                if (background_count < MAX_BACKGROUND_JOBS)
                {
                    background_jobs[background_count++] = pid;
                }
                else
                {
                    fprintf(stderr,
                            "msh: background job list full; waiting for process\n");

                    int status;

                    if (wait_for_foreground(pid, &status) == 0)
                    {
                        last_status = get_exit_status(status);
                    }
                    else
                    {
                        last_status = 1;
                    }
                }
            }
            else
            {
        
         // TASK 5: WAIT FOR FOREGROUND CHILD
        

                int status;

                if (wait_for_foreground(pid, &status) == -1)
                {
                    last_status = 1;
                }
                else
                {
                    last_status = get_exit_status(status);
                }
            }
        }

        // Release the argument pointer array
        free(args);
    }

    // TASK 1: Release memory allocated by getline()
    free(line);

    return 0;
}

