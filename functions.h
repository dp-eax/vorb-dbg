#ifndef FUNC
#define FUNC
#include "includes.h"

int breakpoint(long addr, int pid);
int cont(int pid);
int detach(int pid);
siginfo_t getsiginfo(int pid);
struct user_regs_struct getregs(int pid, int print);
long peekdata(void *addr, int pid);
long pokedata(void *addr, void *data, int pid);
int step(int pid);

#endif
