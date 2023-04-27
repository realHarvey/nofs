/*
 *  include / compiler.h
 *  Copyright(c) 2023  Harvey
 */
#pragma once

#define barrier() __asm ("":::"memory")

/*          CPU barrier
 * -----------------------------
 * LD    ISHLD    OSHLD    NSHLD
 * ST    ISHST    OSHST    NSHST
 * -----------------------------
*/
#define dmb(option) __asm volatile ("dmb " #option ::: "memory")
#define dsb(option) __asm volatile ("dsb " #option ::: "memory")
#define isb(option) __asm volatile ("isb " #option ::: "memory")


#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif

#ifndef __weak
#define __weak __attribute__((weak))
#endif

/*
 * 内核进行共享内存的访问
 * 处在竞争状态的代码
 * 阻止编译器对它们的临时存储 合并 重排
 */
#define REREAD(x) (* (volatile typeof(x) *)&(x))
