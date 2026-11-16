#define _POSIX_C_SOURCE 200809l

#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    char *line = NULL;
size_t len = 0;
ssize_t nread;
while (1)
{
   printf("msh[0]> ");
   fflush(stdout);

   nread = getline(&line, &len,stdin);
      if (nread == -1)
        {
        printf("\n");
        break; 
         }
         printf("you entered: %s", line);
                   }
    		free(line);
return 0;
}
