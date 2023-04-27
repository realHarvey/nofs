/*
 *  panic
 *  Copyright(c) 2023  Harvey
 */
#include "unistd.h"
#include "arm.h"

/*
 * 重大内核错误
 *
 * 关闭除NMI的所有中断
 * TODO: 同步内存数据
 * 进入低功耗循环
 */
void panic(const char *fmg)
{
        cli();
        printk("kernel panic : ");
        printk(fmg);
        while (1) {
                wfi();
        }
}
