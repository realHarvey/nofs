/*
 *  include / arm
 *  Copyright(c) 2023  Harvey
 */
#pragma once

#define sti() __asm ("cpsie i" ::: "memory")
#define cli() __asm ("cpsid i" ::: "memory")

#define nop() __asm ("nop")

#define wfe() __asm ("wfe" ::: "memory")
#define sev() __asm ("sev" ::: "memory")
#define wfi() __asm ("wfi" ::: "memory")

#define svc(n) __asm ("svc %0" :: "i"(n))

/* 手动触发中断号为n的中断 */
#define i_wanna_it(n) \
__asm ("mov r0, %0 \n" \
"ldr r1, =0xE000EF00 \n" \
"str r0, [r1] \n" \
:: "i"(n) : "r0", "r1")

#define move_to_user_mode() \
__asm ("msr control, %0" :: "r"(3))

#define system_mode() ({ \
        int mode; \
        __asm ("mrs %0, control" : "=r"(mode)); \
        mode; \
})
