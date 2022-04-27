#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

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

void removeNewline(char *input){
  int i;
  for (i = 0;  input[i] != '\n' && input[i] != '\0';  i++);
  input[i] = '\0';
}
char **extractCommand2(char *input){
  char **tokens = malloc(sizeof(char*) * MAX_ARGS);

  char *token, *str, *tofree;
  tofree = str = strdup(input);  
  int i = 0;
  while((token = strsep(&str, " "))){
    if(!(token[0] == '\0')){
      tokens[i] = strdup(token);
      i++;
    }
   } 

  // the last last item keeps a \n that breaks stuff
  removeNewline(tokens[i-1]);

  free(tofree);
  return tokens;
}

int launchCommands(char **firstArgs, char**secondArgs){
  int     fd[2];
  pid_t   childpid;
  pipe(fd);

  if((childpid = fork()) == -1) {
      perror("fork");
      exit(1);
  }

  if (childpid == 0){
    close(1);
    dup(fd[1]);

    close(fd[0]);

    return execvp(firstArgs[0], firstArgs);
  } else  {
    close(0);
    dup(fd[0]);

    close(fd[1]);

    return execvp(secondArgs[0], secondArgs);
  }
}

void freeArgs(char **args){
  int i;
  for (i = 0; args[i] != NULL; i++){
    free(args[i]);
  }
  free(args);
}

int main(int argc, char **argv)
{
  char input[MAX_LINE];
  int wstatus, i;
  printf(">> ");
  for (fgets(input, MAX_LINE, stdin); strcmp(input, "exit\n"); fgets(input, MAX_LINE, stdin))
  {

    char *rest;
    char **firstArgs = extractCommand2(strtok_r(input, "|", &rest));
    char **secondArgs = extractCommand2(strtok_r(NULL, "|", &rest));

    pid_t pid = fork();
    if (pid < 0)
    {
      perror("Fork has faield\n");
    }
    else if (pid == 0)
    {
      launchCommands(firstArgs, secondArgs);
    }
    wait(&wstatus);

    printf(">> ");
    // freeArgs(firstArgs);
    // freeArgs(secondArgs);

    fflush(stdin);
  }
  return 0;
}
