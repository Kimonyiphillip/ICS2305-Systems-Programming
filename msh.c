#define _POSIX_C_SOURCE 200809l

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //task2  added
#include <unistd.h>//task3 added
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h> //task4 added
int main(void)
{
    char *line = NULL;
size_t len = 0;
ssize_t nread;
		//argument Storage
		char *args[128];
		int argc;
		int last_status = 0; //task5: store previous command exit status
while (1)
{
   printf("msh[%d]> ", last_status);//display prev command status
   fflush(stdout);
   //read input of any length
   nread = getline(&line, &len,stdin);
		//Task 1 : handle CRTL -D
      if (nread == -1)
        {
        printf("\nExisting Msh....\n");

        break; 
         }

//TASK2 : SPLIT INPUT INTO RGUMENTS
		argc = 0;
		char *token = strtok(line, "\t\n");

		while (token != NULL && argc < 127)
		{ args[argc] = token;
                     argc++;
		token = strtok(NULL, "\t\tn");
		}
			args[argc] = NULL;
		//IGNORE BLANK LINES
		if (argc == 0){
		continue;
				}
				//TASK4 BUILT IN HELP COMMAND
			if (strcmp(args[0], "help") == 0)
				{
					printf("Available commands:\n");
				     printf("cd [directory] - Change directory\n");
			         printf("help - Display this help message\n");
			      printf("exit - exit the shell\n");
						continue;

      				}
				//built in eEXIT command
				if (strcmp(args[0], "cd") == 0)
					{
					char *directory;
					if (args[1] == NULL)
					{
						directory = getenv("HOME");
						}
						else
						{
						    directory = args[1];
						}
				if (directory == NULL || chdir(directory) != 0)
						{
						perror("cd");
							}
						continue;

}

			//TASK3 CODE ADDED CHILD PROCESS
		pid_t pid = fork();
		if (pid < 0){
			//handle fork error
			perror("fork");
			continue;
			}
			if (pid == 0){
				//child executes command
				execvp(args[0], args);
			perror(args[0]);
			exit(EXIT_FAILURE);
			}
			else
			{
			//task 5 added fore ground child 
			int status;
			if (waitpid(pid, &status, 0) == -1)
				{
					perror("waitpid");
					last_status = 1;
					}
					else{
					//normal exit status
					if (WIFEXITED(status)){last_status = WEXITSTATUS(status);}
						else if
						(WIFSIGNALED(status)){last_status = 128 + WTERMSIG(status);}
						}
			}

		 }
		//RELEASE ALLOCTED MEMORY
		free(line);
		return 0;
}


