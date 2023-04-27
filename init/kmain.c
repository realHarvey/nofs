/*
 *  kmain.c
 *  内核初始化
 *  Copyright(c) 2023  Harvey
 */
#include "sys.h"
#include "timer.h"
#include "sched.h"
#include "arm.h"
#include "unistd.h"

extern void uinit();

/*
 * 不能在调度器初始化之前禁掉中断
 * 否则spinlock将无法退出
 */
void kmain(void)
{
        // 硬件资源初始化
        sysclk_init();
        timer_init();
        sys_uart_init();
        nvic_init();

        // 系统内核初始化
        sched_init();
        task_init();
        sched_add_task(&init);
        current = &init;

        // 进入主进程
        sti();
        move_to_user_mode();
        uinit();
}

void uinit()
{
        int test_pid = fork();
        if (test_pid) {

        }
        while (1);
}