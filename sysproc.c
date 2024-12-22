#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

//task 5
#include "pstat.h"


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//task 4
int sys_getreadcount(void) {
    extern int readcount; // Reference the global counter
    return readcount;
}

//task 5
int sys_settickets(void) {
  int tickets;
  if (argint(0, &tickets) < 0 || tickets < 1) {
    return -1; // Invalid input
  }
  proc->tickets = tickets; // Set tickets for current process
  return 0;
}

int sys_getpinfo(void) {
  struct pstat *ps;
  if (argptr(0, (void*)&ps, sizeof(*ps)) < 0) {
    return -1; // Invalid pointer
  }

  acquire(&ptable.lock);
  for (int i = 0; i < NPROC; i++) {
    ps->inuse[i] = ptable.proc[i].state != UNUSED;
    ps->tickets[i] = ptable.proc[i].tickets;
    ps->pid[i] = ptable.proc[i].pid;
    ps->ticks[i] = ptable.proc[i].ticks;
  }
  release(&ptable.lock);
  return 0;
}
