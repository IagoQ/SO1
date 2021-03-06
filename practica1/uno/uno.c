#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 4096
#define MAX_ARGS 2048

char *aux(char *str, int l, int r)
{
  char *rta = malloc(r - l + 1);
  for (int i = l; i < r; i++)
  {
    rta[i - l] = str[i];
  }
  rta[r - l] = '\0';
  return rta;
}

char **extractCommand(char *input){

  char **list = malloc(sizeof(char *) * MAX_ARGS);
  int l = 0, r = 0, c = 0, i;

  if (input[0] == ' '){
    for(l = 0; input[l] == ' ';l++);
  }

  for (i = l; input[i] != '\0'; i++)
  {
    if (input[i] == ' ' || input[i] == '\n')
    {
      r = i;
      list[c] = aux(input, l, r);
      l = i + 1;
      c++;
    }
  }
  list[i++] = NULL;
  return list;
}

int executeToFile(char **firstArgs, char*filepath){
  int outputFd = open(filepath, O_CREAT | O_WRONLY, 0644);

    // close stdout, dup pipein to override it
    close(1);
    dup(outputFd);

    execv(firstArgs[0], firstArgs);
}

void freeArgs(char **args){
  int i;
  for (i = 0; args[i] != NULL; i++)
  {
    free(args[i]);
  }
  free(args[i]);
  free(args);
}

int main(int argc, char **argv)
{
  char input[MAX_LINE];
  int wstatus, i;
  printf(">> ");
  for (fgets(input, MAX_LINE, stdin); strcmp(input, "exit\n"); fgets(input, MAX_LINE, stdin))
  {

    char **firstArgs = extractCommand(strtok(input, ">"));
    char **secondArgs = extractCommand(strtok(NULL, ">"));

    pid_t pid = fork();
    if (pid < 0)
    {
      perror("Fork has faield\n");
    }
    else if (pid == 0)
    {
      executeToFile(firstArgs, secondArgs[0]);
    }
    wait(&wstatus);

    printf(">> ");
    freeArgs(firstArgs);
    freeArgs(secondArgs);

    fflush(stdin);
  }
  return 0;
}
