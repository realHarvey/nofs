/*
 *  syscall.c
 *  Copyright(c) 2023  Harvey
 */
#include "syscall.h"
#include "sched.h"

int sys_fork(void)
{
        return do_fork();
}

int sys_exit(void)
{
        do_exit();
        return 0;
}

static int (*syscalls[])(void) = {
        [SN_fork] = sys_fork,
        [SN_exit] = sys_exit,

};

void syscall(void)
{
        int num;
        __asm volatile (
        "mov %0, r0"
        :"=&r"(num)
        ::);
        syscalls[num]();
}