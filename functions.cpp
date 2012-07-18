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
  long data = peekdata((void*)addr);
  breakpoints[num_breakpoints][0] = addr;
  breakpoints[num_breakpoints][1] = data;
  num_breakpoints++;
  pokedata((void*)addr, (void*)0xcccccccc);
  data = peekdata((void*)addr);
  printf("%lx\n", data);
}

int dbg::rm_breakpoint(long addr, int hit)
{
  for(int i=0;i < num_breakpoints; i++)
  {
    if(breakpoints[i][0] == addr)
    {
      pokedata((void*)addr, (void*)breakpoints[i][1]);
      printf("Removed breakpoint %d at %lx, data is now: %lx\n", i, addr, peekdata((void*)addr));
      i++;
      for(i; i < num_breakpoints; i++)
      {
        breakpoints[i-1][0] = breakpoints[i][0];
        breakpoints[i-1][1] = breakpoints[i][1];
      }
      num_breakpoints--;
      if(hit == 1)
      {
        regs.eip = regs.eip - 1;
        ptrace(PTRACE_SETREGS, pid, NULL, &regs);
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
