/*
 *   vorb-dbg, functions.h
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

#ifndef FUNC
#define FUNC
#include "includes.h"

class dbg
{
  int pid;
  int attached;
  struct user_regs_struct regs;
  siginfo_t siginfo;
  long breakpoints[8][2];
  int num_breakpoints;

  public:
    dbg(int init_pid, char *file_name, char *arguments[], int argc)
    {
      attached = 1;
      num_breakpoints = 0;
      pid = init_pid;
      if(pid == 0)
        create_process(file_name, arguments, argc);
      else
        attach();
    }

    int attach();
    int breakpoint(long addr);
    int cont();
    int create_process(char *file_name, char *arguments[], int argc);
    int detach();
    int getcmd(char *cmd);
    int getsiginfo();
    int getregs(int print);
    long peekdata(void *addr);
    int pokedata(void *addr, void *data);
    int rm_breakpoint(long addr, int hit);
    int list_breakpoints();
    int step();
};

#endif
