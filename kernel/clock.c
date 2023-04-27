/*
 *  clock.c
 *  Copyright(c) 2023  Harvey
 */
#include "sys.h"
#include "compiler.h"
#include "../.config"

#ifdef CPU_CLK_MHZ
#if CPU_CLK_MHZ == 72
#define PLL_FACTOR 0x1C0000
#elif CPU_CLK_MHZ == 64
#define PLL_FACTOR 0x180000
#elif CPU_CLK_MHZ == 56
#define PLL_FACTOR 0x140000
#elif CPU_CLK_MHZ == 48
#define PLL_FACTOR 0x100000
#else
#error "时钟频率不是8的系数"
#endif
#else
#define PLL_FACTOR 0x1C0000
#endif

struct _rcc {
        unsigned      CR;
        unsigned      CFGR;
        unsigned      CIR;
        unsigned      APB2RSTR;
        unsigned      APB1RSTR;
        unsigned      AHBENR;
        unsigned      APB2ENR;
        unsigned      APB1ENR;
        unsigned      BDCR;
        unsigned      CSR;
};

#define rcc ((struct _rcc *) 0x40021000)

/*
 *  系统时钟
 * (默认配置)
 * CPU频率 : 72MHz
 * 时基单元 : 72MHz
 * 高速总线 : 72MHz
 * 外设总线 : (2)72MHz. (1)36MHz
 */
void sysclk_init()
{
/* 启动HSE 并等待启动完成
 * PLL设定输入时钟源HSE 并9倍频
 * 启动PLL 并等待锁定
 * 选择系统时钟为PLL
 * 开启ram和flash时钟并开启复位
 * APB2高速总线1分频 > sys_uart线路时钟
 * APB1低速总线2分频
 */     rcc->CR &= 0;
        rcc->CR |= 0x10000;
        while ( !(REREAD(rcc->CR) & 0x20000) );
        rcc->CFGR |= 0x10000;
        rcc->CFGR |= PLL_FACTOR;
        rcc->CR |= 0x1000000;
        while ( !(REREAD(rcc->CR) & 0x2000000) );
        rcc->CFGR |= 0b10;
        while ( (REREAD(rcc->CFGR) & 0b1100) != 0b1000 );
        rcc->AHBENR |= 0x14;
        rcc->CSR |= 0x1C000000;
        rcc->APB2ENR |= 0x4025;
        rcc->CFGR |= 0x400;
        rcc->APB1ENR |= 0x10000000;
}
