#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

// 包含sysinfo的头文件。
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
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

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// 将参数保存到proc结构体里的一个新变量中来实现新的系统调用。
uint64
sys_trace(void){
 
  int n;

// argint: Fetch the nth 32-bit system call argument.
  if(argint(0, &n) < 0)
    return -1;
  // 将mask 保存在本进程的 proc结构体中。
  myproc()->trace_mask=n;
  return 0;
 
}

//系统调用 sysinfo
uint64
sys_sysinfo(void){
    //将一个struct sysinfo复制回用户空间
    uint64 addr;
    if(argaddr(0,&addr)<0)
      return -1;
    
    struct sysinfo info;
  
    info.freemem=get_free_mem();
    info.nproc=get_proc_num();

    // Copy from kernel to user.
    // Copy len bytes from src to virtual address dstva in a given page table.
    // Return 0 on success, -1 on error.
    //函数原型 int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len) 
    if (copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0) 
        return -1;
    return 0;
}