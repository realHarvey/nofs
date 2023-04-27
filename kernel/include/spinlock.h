/*
 *  include / spinlock
 *  Copyright(c) 2023 Harvey
 */
#pragma once
#include "compiler.h"
#include "arm.h"
#include "sched.h"
/*
 * spinlock
 *
 * tickets :
 *      number -> 当前线程被分配的号码
 *      next   -> 下一个可获得锁的号码
 *      ------
 *      争锁 :
 *          thr申请锁, 它的number = 原number + 1
 *          number == next, 代表当前thr获得spinlock的资格, 同时其他thr仍被阻塞
 *      ------
 *      解锁 :
 *          next + 1, 把票号推给下一个等待队列中的线程
 * lock :
 *      lock != 0, 锁已被占有
 *
 * ! __ARMEL__下,number占据低四位, next占高四位
 */
typedef struct {
union {
        struct {
            unsigned char number;
            unsigned char next;
        } tickets;
        unsigned short lock;
};
}spinlock_t;

static inline void spin_init(spinlock_t *spin)
{
        barrier();
        // next的初始值必须为1 : task1
        spin->lock = 0x0100;
        barrier();
}

static inline void spin_lock(spinlock_t *spin)
{
        spinlock_t origin;
        unsigned int newval;
        __asm volatile (
        "1: ldrex %0, [%2] \n"
        "   add %1, %0, #1 \n"
        "   strex %0, %1, [%2] \n"
        "   teq %0, #0 \n"
        "   bne 1b"
        :"=&r"(origin), "=&r"(newval)
        :"r"(&spin->lock)
        :"cc" );
        // 线程尝试获取锁时, number的值保存在寄存器中, 不会从内存中重取
        // 因此, 这个number就是此线程获取锁时得到的票号
        while (spin->tickets.number != spin->tickets.next) {
                wfi();
                spin->tickets.next = REREAD(spin->tickets.next);
        }
        barrier();
}

static inline void spin_unlock(spinlock_t *spin)
{
        barrier();
        spin->tickets.next ++;
        __asm ("dsb ishst");
}

static inline int spin_is_locked(const spinlock_t *spin)
{
        spinlock_t _spin = REREAD(*spin);
        return _spin.tickets.number == _spin.tickets.next;
}

