/*
 *  tty
 *  Copyright(c) 2023  Harvey
 */
#include "unistd.h"
#include "../.config"

#define SR_TXE  0x80
#define SR_TC   0x40
#define SR_RXNE 0x20

struct _uart {
        unsigned      SR;
        unsigned      DR;
        unsigned      BRR;
        unsigned      CR1;
        unsigned      CR2;
        unsigned      CR3;
        unsigned      GTPR;
};
#define uart ((struct _uart *) 0x40013800)
#define GPIOA_CRH (*((unsigned int *)0x40010804))

/*
 * uart1作为nofs的系统串口
 * 用于debug的输入输出
 * 波特率 : 115200
 * pin : (out)PA9 (in)PA10
 */
void sys_uart_init()
{
        GPIOA_CRH &= ~0xF0;
        GPIOA_CRH |= 0xB0;
        uart->CR2 = 0;
        uart->CR3 = 0;
        uart->BRR = 0x271;
        uart->CR1 = 0x200C;
}

static void uart_transmit(const char *chr)
{
        while ((unsigned char)(*chr)) {
                while (!(uart->SR & SR_TXE));
                uart->DR = (unsigned char)(*chr++ & 0xff);
        }
        while (!(uart->SR & SR_TC));
}

void printk(const char *msg)
{
        uart_transmit(msg);
}

/*
 * tty指令数据接收缓冲
 * TODO : 规定指令和处理程序
 */
unsigned char tty_io_buffer[TTY_BUF_SIZE];
void tty_io_interrupt()
{
        int p = 0;
        while (1) {
                tty_io_buffer[p++] = uart->DR;
                if (!(uart->SR & SR_RXNE))
                        break;
        }
}
