#include "functions.h"

int dbg::breakpoint(long addr)
{
  printf("Inserting breakpoint at %x\n", addr);
  long data = peekdata((void*)addr);
  printf("%lx\n", data);
  pokedata((void*)addr, (void*)0xcccccccc);
  data = peekdata((void*)addr);
  printf("%lx\n", data);
}

int dbg::cont()
{
  return ptrace(PTRACE_CONT, pid, NULL, NULL);
}

int dbg::detach()
{
  ptrace(PTRACE_DETACH, pid, NULL, NULL);
  printf("Detached from %d\n", pid);
}

int dbg::getregs(int print)
{
  ptrace(PTRACE_GETREGS, pid, NULL, &regs);
  if(print == 0)
    printf("EAX: %x\nEBX: %x\nECX: %x\nEDX: %x\nESP: %x\nEIP: %x\nESI: %x\nEDI: %x\nEBP: %x\n", 
      regs.eax, regs.ebx, regs.ecx, regs.edx, regs.esp, regs.eip, regs.esi, regs.edi, regs.ebp);
}

int dbg::getsiginfo()
{
  ptrace(PTRACE_GETSIGINFO, pid, NULL, &siginfo);
}

long dbg::peekdata(void *addr)
{
  return ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
}

int dbg::pokedata(void *addr, void *data)
{
  ptrace(PTRACE_POKEDATA, pid, addr, data);
}

int dbg::step()
{
  ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
}
