/*
 *  sched.c
 *  Copyright(c) 2023  Harvey
 */
#include "sched.h"
#include "list.h"
#include "spinlock.h"
#include "timer.h"
#include "syscall.h"
#include "unistd.h"
#include "stddef.h"
#include "../.config"

static int last_pid = 1;
struct task *current;

struct {
        struct klist task_queue;
        spinlock_t spin;
}sched;

void idle()
{
        while (1) {
                wfi();
                break;
        }
}

void sched_init()
{
        spin_init(&sched.spin);
        spin_lock(&sched.spin);
        list_init(&sched.task_queue);
        spin_unlock(&sched.spin);
        current = NULL;
}

void sched_add_task(struct task *task)
{
        /* 不允许调度僵死进程 */
        if (task->state == TASK_DIED)
                return;
        spin_lock(&sched.spin);
        list_add_tail(&task->task_link, &sched.task_queue);
        spin_unlock(&sched.spin);
}

#ifdef LONG_TIME_RUNNING
/* 如果系统需要长时间不间断运行，则启用此策略 */
static int alloc_pid()
{
        int i, pcount;
        struct task *iter;
        int pid_exist[MOST_TASKS] = { 0 };
/*
 * 统计非死进程的数量
 * 如果进程数达到限制且都不处于僵死态，则不予分配新进程
 */     i = 0; pcount = 0;
        list_for_each_container(&sched.task_queue, iter, task_link){
                if (iter->state == TASK_DIED)
                        pcount--;
                else
                        pcount++;
                pid_exist[i] = iter->pid;
                i++;
        }
        if (pcount == MOST_TASKS)
                return -1;
        if ( ++last_pid < 0 )
                last_pid = 2;
        for (i = 0; i < MOST_TASKS; i++)
                if (last_pid == pid_exist[i])
                        last_pid++;
        return last_pid;
}
#else
static int alloc_pid()
{
        return ++last_pid;
}
#endif

/*
static long *kstack_alloc()
{
        static long alloc_base = 0x20002800UL;
        alloc_base -= 0x500;
        if (alloc_base < 0x20002800UL - MOST_TASKS * 0x500)
            return 0;
        return (void *)alloc_base;
} */

int do_fork(void)
{
        int pid;
        struct task cp;
        LIST_INIT(task_link);

        pid = alloc_pid();
        cp = (struct task) {
                .pid = pid, .parent = current,
                .state = TASK_READY,
                .prio = current->prio, .count = current->count++, //TODO: 根据类型分配优先级 // 暂时先让新进程的count更大一点便于快速投入运行
                .st = current_time(), .rt = 0,
                .task_link = task_link,
                .context = current->context
        };
        sched_add_task(&cp);
        //TODO：fork后立即切换 / 等待schedule
        if(pid == current->pid)
                pid = 0;
        return pid;
}

void do_exit(void)
{
        struct task *ep;
        list_for_each_container(&sched.task_queue, ep, task_link)
                if (ep->state == TASK_RUNNIN)
                        break;
        // 禁止结束init进程
        if (ep->pid == 1)
                oops("failed to exit task init");
        spin_lock(&sched.spin);
        list_del(&ep->task_link);
        spin_unlock(&sched.spin);
        ep->state = TASK_DIED;
        schedule();
}


void schedule()
{
        int c;
        struct task *iter, *next;

        while (1) {
        /*
         * 寻找状态为ready且可运行时间片最大的进程
         * 切换到它运行
         * 如果没有可以运行的进程, 则重新分配时间片
         */     c = -1;
                next = NULL;
                cli();
                list_for_each_container(&sched.task_queue, iter, task_link)
                        if (iter->state == TASK_READY && iter->count > c) {
                                c = iter->count;
                                next = iter;
                        }
                sti();
                if (next->pid) break;
                /*
                 * 对于部分被阻塞的进程, 我们仍保留其剩余时间片在权值计算中的作用
                 * 不过随着阻塞时常的增加, 其权值会越来越少
                 */
                list_for_each_container(&sched.task_queue, iter, task_link)
                        iter->count = iter->prio + (iter->count>>1);
        }
        task_switch(current, next);
}