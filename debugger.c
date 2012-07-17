// dbg.c
#include "includes.h"
#include "functions.h"

int attach(int pid);
int create_process(char *file_name, char *arguments[], int argc);
int getcmd(char *cmd, int pid);
void usage(char *program_name);

int attach(int pid)
{
  int status;
  ptrace(PTRACE_ATTACH, pid, NULL, NULL);
  waitpid(pid, &status, WUNTRACED);
  cont(pid);
}

int create_process(char *file_name, char *arguments[], int argc)
{
  printf("Debugging %s, press enter to continue\n", file_name);
  char *argument[1024] = { file_name };
  char cmd[100];
  int status, cpid;
  siginfo_t siginfo;
  struct stat sb;
  struct user_regs_struct regs;

  for(int x=3; x<argc; x++)
    argument[x-2] = arguments[x];
  argument[argc-2] = NULL;

  if(stat(file_name, &sb) != 0)
  {
    perror("stat(): ");
    exit(0);
  }

  if((cpid = fork()) == 0)
  {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    if(execv(file_name, argument) == -1)
    {
      perror("execv(): ");
      exit(0);
    }
  }
  else
  {
    for(;;)
    {
      wait(&status);
      while(getcmd(cmd, cpid) != 1) {}
      regs = getregs(cpid, 1);
      printf("Continuing from: %x\n", regs.eip);
      if(WIFEXITED(status))
      {
        printf("Process exited\n");
        break;
      }
      siginfo = getsiginfo(cpid);
      printf("Caught %s\n", strsignal(siginfo.si_signo));
      cont(cpid);
    }
    detach(cpid);
  }
  return 0;
}

int getcmd(char *cmd, int pid)
{
  char *pch, *cmd_split[10], **endptr;
  int i=0;
  printf("> ");
  fgets(cmd, 99, stdin);
  pch = strtok(cmd, " ");
  while (pch != NULL)
  {
    cmd_split[i] = pch;
    pch = strtok(NULL, " ");
    i++;
  }

  if(strlen(cmd_split[i-1]) != 1)
    cmd_split[i-1][strlen(cmd_split[i-1])-1] = '\0';

  if(strcmp(cmd_split[0], "detach") == 0)
  {
    detach(pid);
    return 0;
  }
  else if(strcmp(cmd_split[0], "break") == 0 && i == 2)
  {
    breakpoint(strtoll(cmd_split[1], NULL, 16), pid);
    return 0;
  }
  else if(strcmp(cmd_split[0], "regs") == 0)
  {
    getregs(pid, 0);
    return 0;
  }
  else if(strcmp(cmd_split[0], "set") == 0 && i == 3)
  {
    pokedata((void *)(int)strtoll(cmd_split[1], NULL, 16), (void *)(int)strtoll(cmd_split[2], NULL, 16), pid);
    printf("%s: %lx\n", cmd_split[1], peekdata((void *)(int)strtoll(cmd_split[1], NULL, 16), pid));
    return 0;
  }
  else if(strcmp(cmd_split[0], "print") == 0 && i == 2)
  {
    printf("%s: %lx\n", cmd_split[1], peekdata((void *)(int)strtoll(cmd_split[1], NULL, 16), pid));
    return 0;
  }
  else if(strcmp(cmd_split[0], "h") == 0 || strcmp(cmd_split[0], "help") == 0)
  {
    printf("detach               -- detach from process\n");
    printf("break <address>      -- set breakpoint\n");
    printf("regs                 -- print registers\n");
    printf("set <address> <data> -- set address to data\n");
    printf("print <address>      -- print data at address\n");
    printf("continue             -- continue process\n");
    printf("help                 -- print this message\n");
    return 0;
  }
  else if(strcmp(cmd_split[0], "c") == 0 || strcmp(cmd_split[0], "continue") == 0 || strcmp(cmd_split[0], "\n") == 0)
  {
    return 1;
  }

  else
  {
    printf("Unknown command, type help.\n");
    return 0;
  }
}

void usage(char *program_name)
{
  printf("Usage: %s [option] [options]\n", program_name);
  printf(" -a/--attach <pid> -- attach to process\n");
  printf(" -c/--create <create process> <process arguments> -- create process\n");
  printf(" -h/--help -- display this message\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  if(argc < 2)
    usage(argv[0]);

  if(strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--create") == 0)
    create_process(argv[2], argv, argc);
  else if(strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--attach") == 0)
    attach(atoi(argv[2]));
  else
    usage(argv[0]);
}
