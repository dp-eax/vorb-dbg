/*
 *   vorb-dbg, functions.cpp
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

#include "functions.h"

int dbg::breakpoint(long addr)
{
  printf("Inserting breakpoint %d at %x\n", num_breakpoints, addr);
  long data = peekdata((void*)addr, 0);
  printf("%x\n", data);
  breakpoints[num_breakpoints][0] = addr;
  breakpoints[num_breakpoints][1] = data;  // 0x08046354
  asm ("push $0xcccccccc\n\t"
       "push %1\n\t"
       "pop %%eax\n\t"
       "rol $0x8, %%eax\n\t"
       "push %%eax\n\t"
       "inc %%esp\n\t"
       "pop %0\n\t"
       :"=r"(data)
       :"r"(data));
  num_breakpoints++;
  pokedata((void*)addr, (void*)data);
  printf("%x\n", peekdata((void*)addr, 0));
}

int dbg::rm_breakpoint(long addr, int hit)
{
  for(int i=0;i < num_breakpoints; i++)
  {
    if(breakpoints[i][0] == addr)
    {
      pokedata((void*)addr, (void*)breakpoints[i][1]);
      if(hit == 1)
      {
        step();
        printf("%x\n", breakpoints[i][1]);
        breakpoint(breakpoints[i][0]);
        //regs.eip = regs.eip - 1;
        //ptrace(PTRACE_SETREGS, pid, NULL, &regs);
      }
      else
      {
        printf("Removed breakpoint %d at %lx, data is now: %lx\n", i, addr, peekdata((void*)addr, 0));
        i++;
        for(i; i < num_breakpoints; i++)
        {
          breakpoints[i-1][0] = breakpoints[i][0];
          breakpoints[i-1][1] = breakpoints[i][1];
        }
        num_breakpoints--;
      }
      break;
    }
  }
}

int dbg::list_breakpoints()
{
  for(int i=0; i < num_breakpoints; i++)
    printf("Breakpoint %d: %lx\n", i, breakpoints[i][0]);
}

int dbg::cont()
{
  return ptrace(PTRACE_CONT, pid, NULL, NULL);
}

int dbg::detach()
{
  ptrace(PTRACE_DETACH, pid, NULL, NULL);
  printf("Detached from %d\n", pid);
  attached = 0;
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

long dbg::peekdata(void *addr, int numwords)
{
  if(numwords == 0)
    return ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
  else
  {
    int j = 0;
    for(int i=0; i < numwords*4; i=i+4)
    {
      if(j == 0)
        printf("%lx: ", (long)addr+i);
      printf(" %.8lx", ptrace(PTRACE_PEEKDATA, pid, (void*)((long)addr+i), NULL));
      j++;
      if(j == 11)
      {
        printf("\n");
        j = 0;
      }
    }
    if(j != 0)
      printf("\n");
  }
}

int dbg::pokedata(void *addr, void *data)
{
  ptrace(PTRACE_POKEDATA, pid, addr, data);
}

int dbg::step()
{
  ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
}
