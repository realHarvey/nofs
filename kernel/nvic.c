/*
 *  nvic
 *  Copyright(c) 2023  Harvey
 */
#include "sys.h"

#define SETENR   ((unsigned int *) 0xE000E100)
#define CLRENR   ((unsigned int *) 0xE000E180)
#define SETPENDS ((unsigned int *) 0xE000E200)
#define CLRPENDS ((unsigned int *) 0xE000E280)
#define EXI_PRI  ((unsigned char*) 0xE000E400)
#define INT_PRI  ((unsigned char*) 0xE000ED18)

/*
 * 系统异常优先级
 * 最高为panic, 异常优先级0
 * 其他系统异常优先级均为1或2
 * 外部中断优先级不可优先于任何系统异常优先级
 */
void nvic_init()
{
/*
 * 配置系统中断异常优先级
 *
 * 开启37号sys_uart中断服务
 * 优先级 : 5
 */     INT_PRI[7] = 2; // swi
        INT_PRI[11] = 1; // timer_interrupt
        SETENR[1] = 0x20;
        EXI_PRI[37] = 5;
}
