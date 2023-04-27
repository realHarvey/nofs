/*
 *  trap
 *  Copyright(c) 2023  Harvey
 */
#include "unistd.h"
/*
 * 尝试对内核做出威胁时触发oops
 * 对用户程序发出警告并终止危险行为
 */
void oops(char *msg)
{
        printk("Oops! : ");
        printk(msg);
}
