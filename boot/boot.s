/*
 *  meboot
 *  Copyright(c) 2023  Harvey
 */
.syntax unified
.text
/*
 * 所有进程共享一个512bytes的内核栈以及一个5kb的堆
 * 每个进程单独分配一个1kb的用户栈
 */
.equ RAMADDR, 0x20002800
.equ KSTACK, 0x200
.equ USTACK, 0x400
.equ FLASHADDR, 0x40022000

.globl start
start :
        .type start, %function
        ldr r0, =RAMADDR
        msr msp, r0
        mrs r0, msp
        sub r0, #KSTACK
        msr psp, r0

        ldr r0, =startof_data
        ldr r1, =endof_data
        ldr r2, =flash_addr_data
        mov r3, #0
1:
        cmp r4, r1
        bcc copy_data_to_ram

        ldr r2, =startof_bss
        ldr r4, =endof_bss
        mov r3, #0
2:
        cmp r2, r4
        bcc startup_bss
/*
 * 保持PB区和无半周期
 * 设置2倍访问延迟
 */     ldr r0, =FLASHADDR
        mov r1, 0b10010
        str r1, [r0]

        bl kmain

copy_data_to_ram :
        ldr r4, [r2, r3]
        str r4, [r0, r3]
        add r3, r3, #4
        add r4, r0, r3
        b 1b

startup_bss :
        str r3, [r2]
        add r2, r2, #4
        b 2b
