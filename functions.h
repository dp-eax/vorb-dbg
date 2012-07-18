#ifndef FUNC
#define FUNC
#include "includes.h"

class dbg
{
  int pid;
  struct user_regs_struct regs;
  siginfo_t siginfo;

  public:
    dbg(int pid, char *file_name, char *arguments[], int argc)
    {
      if(pid == 0)
        create_process(file_name, arguments, argc);
      else
        attach(pid);
    }

    int attach(int pid);
    int breakpoint(long addr);
    int cont();
    int create_process(char *file_name, char *arguments[], int argc);
    int detach();
    int getcmd(char *cmd);
    int getsiginfo();
    int getregs(int print);
    long peekdata(void *addr);
    int pokedata(void *addr, void *data);
    int step();
};

#endif
