#define _POSIX_C_SOURCE 200809l

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //task added
int main(void)
{
    char *line = NULL;
size_t len = 0;
ssize_t nread;
		//argument Storage
		char *args[128];
		int argc;
while (1)
{
   printf("msh> ");
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
		//display args for testing
		printf("argumnts received: \n");
		for (int i = 0; i < argc; i++)
			{
			printf("args[%d] = %s\n", i ,args[i]);
			}
		 }
		//RELEASE ALLOCTED MEMORY
		free(line);
		return 0;
}

