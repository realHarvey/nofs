/*
 *  timer.c
 *  Copyright(c) 2023  Harvey
 */
#include "timer.h"
#include "sched.h"
#include "spinlock.h"
#include "../.config"

#define SYSCLK_FREQ  (CPU_CLK_MHZ<<6)

/*
 * reload最大值 -> 0xFFF_FFF
 */
struct systimer {
    volatile long ctrl;
    volatile long reload;
    volatile long val;
    volatile long calib;
};

#define ticker ((struct systimer *)0xE000E010)

static unsigned int ticks;
static spinlock_t tickspin;

/*
 * timer_init
 *      系统定时器初始化
 *      为调度提供10ms的时间片
 *
 * timer_interrupt
 *      定时器例行中断的执行程序
 *      用于累积开机时间
 *      以及调度进程
 */
void timer_init()
{
        ticker->reload = SYSCLK_FREQ / 100000 - 1;
        ticker->val = 0;
        ticker->ctrl = 0b111;

        spin_init(&tickspin);
        ticks = 0;
}

void timer_interrupt()
{
        spin_lock(&tickspin);
        ticks ++;
        spin_unlock(&tickspin);
        if(ticks % 10 != 9) return;
        schedule();
}

/*
 * 获取当前时间
 * 由于无RTC时钟, 只能获取距离开机的时间
 */
unsigned int current_time()
{
       return ticks;
}