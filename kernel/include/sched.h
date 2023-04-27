/*
 *  include / sched.h
 *  Copyright(c) 2023  Harvey
 */
#pragma once
#include "list.h"

/* state */
#define  TASK_READY    0x01
#define  TASK_RUNNIN   0x02
#define  TASK_WAIT     0x04
#define  TASK_DIED     0x08

/*
 * 由于nofs的进程被设计为共享一个内核栈
 * 我们不能使用内核栈来保存上下文
 * 所以选择将其保存在PCB中
 * 并且, 由于r0-r3 fp lr pc cpsr是被调用者保存
 * 会被汇编器自动保存到用户栈
 * 因此context设计中没有包含它们
 */
struct context {
        long         r4;
        long         r5;
        long         r6;
        long         r7;
        long         r8;
        long         r9;
        long         r10;
        long         r11;
        long         psp;
};


struct task {
/* id */
        int                pid;
        struct task       *parent;

        unsigned char      state;

/* schedule dependence */
        unsigned char      prio;
        unsigned short     count; /* time slices */

/* time */
        unsigned int       st;  /* start time */
        unsigned int       rt;  /* running time */

/* task_link as node of schedule queue */
        struct klist       task_link;
        struct context     context;
};


extern struct task *current;
/*
 * 记录1号进程资源的pcb
 * init为整个系统的第一个进程, 不可被杀死
 * 系统空闲时调用idle
 */
#define task_init() \
struct task init = { \
/* id */ \
        .pid = 1, \
        .parent = 0, \
        .state = TASK_READY, \
/* nice */ \
        .prio = 5, \
        .count = 0, \
/* time */ \
        .st = current_time(), \
        .rt = 0, \
/* sched queue */ \
        .task_link = 0, \
        .context = { .psp = 0x20002600 } \
}


void sched_init(void);
void sched_add_task(struct task *);
void schedule(void);

void task_switch(struct task *, struct task *);

int do_fork(void);
void do_exit(void);

