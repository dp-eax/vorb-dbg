/*
 *   vorb-dbg, debugger.cpp
 *   (C) 2012 vorbis / rorschach
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "includes.h"
#include "functions.h"

void usage(char *program_name);

int dbg::attach()
{
  int status;
  char cmd[100];
  ptrace(PTRACE_ATTACH, pid, NULL, NULL);
  for(;;)
  {
    waitpid(pid, &status, WUNTRACED);
    while(getcmd(cmd) != 1) {}
    getregs(1);
    printf("Continuing from: %x\n", regs.eip);
    if(WIFEXITED(status))
    {
      printf("Process exited\n");
      break;
    }
    getsiginfo();
    printf("Caught %s\n", strsignal(siginfo.si_signo));
    cont();
  }
}

int dbg::create_process(char *file_name, char *arguments[], int argc)
{
  printf("Debugging %s, press enter to continue\n", file_name);
  char *argument[1024] = { file_name };
  char cmd[100];
  int status;
  struct stat sb;

  for(int x=3; x<argc; x++)
    argument[x-2] = arguments[x];
  argument[argc-2] = NULL;

  if(stat(file_name, &sb) != 0)
  {
    perror("stat(): ");
    exit(0);
  }

  if((pid = fork()) == 0)
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
      while(getcmd(cmd) != 1) {}
      getregs(1);
      printf("Continuing from: %x\n", regs.eip);
      if(WIFEXITED(status))
      {
        printf("Process exited\n");
        break;
      }
      getsiginfo();
      printf("Caught %s\n", strsignal(siginfo.si_signo));
      cont();
    }
    detach();
  }
  return 0;
}

int dbg::getcmd(char *cmd)
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
    detach();
    return 0;
  }
  else if(strcmp(cmd_split[0], "break") == 0 && i == 2)
  {
    breakpoint(strtoll(cmd_split[1], NULL, 16));
    return 0;
  }
  else if(strcmp(cmd_split[0], "regs") == 0)
  {
    getregs(0);
    return 0;
  }
  else if(strcmp(cmd_split[0], "set") == 0 && i == 3)
  {
    pokedata((void *)(int)strtoll(cmd_split[1], NULL, 16), (void *)(int)strtoll(cmd_split[2], NULL, 16));
    printf("%s: %lx\n", cmd_split[1], peekdata((void *)(int)strtoll(cmd_split[1], NULL, 16)));
    return 0;
  }
  else if(strcmp(cmd_split[0], "print") == 0 && i == 2)
  {
    printf("%s: %lx\n", cmd_split[1], peekdata((void *)(int)strtoll(cmd_split[1], NULL, 16)));
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
    dbg vorb_dbg(0, argv[2], argv, argc);
  else if(strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--attach") == 0)
    dbg vorb_dbg(atoi(argv[2]), NULL, NULL, NULL);
  else
    usage(argv[0]);
}
