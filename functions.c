#include "functions.h"

int breakpoint(long addr, int pid)
{
  printf("Inserting breakpoint at %x\n", addr);
  long data = peekdata((void*)addr, pid);
  printf("%lx\n", data);
  pokedata((void*)addr, (void*)0xcccccccc, pid);
  data = peekdata((void*)addr, pid);
  printf("%lx\n", data);
}

int cont(int pid)
{
  return ptrace(PTRACE_CONT, pid, NULL, NULL);
}

int detach(int pid)
{
  ptrace(PTRACE_DETACH, pid, NULL, NULL);
  printf("Detached from %d\n", pid);
}

struct user_regs_struct getregs(int pid, int print)
{
  struct user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, NULL, &regs);
  if(print == 0)
    printf("EAX: %x\nEBX: %x\nECX: %x\nEDX: %x\nESP: %x\nEIP: %x\nESI: %x\nEDI: %x\nEBP: %x\n", regs.eax, regs.ebx, regs.ecx, regs.edx, regs.esp, regs.eip, regs.esi, regs.edi, regs.ebp);
  return regs;
}

siginfo_t getsiginfo(int pid)
{
  siginfo_t siginfo;
  ptrace(PTRACE_GETSIGINFO, pid, NULL, &siginfo);
  return siginfo;
}

long peekdata(void *addr, int pid)
{
  return ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
}

long pokedata(void *addr, void *data, int pid)
{
  ptrace(PTRACE_POKEDATA, pid, addr, data);
}

int step(int pid)
{
  ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
}
