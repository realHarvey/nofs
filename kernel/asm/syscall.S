#include "syscall.h"
.syntax unified
.text

.globl fork
fork :
        svc #SN_fork
        bx lr


.globl exit
exit :
        svc #SN_exit
        bx lr


.globl do_svc
do_svc :
        .type do_svc, %function
        stmdb sp!, {r4-r11}
        mrs r0, psp
        add r1, r0, #24
        ldr r2, [r1]
        // thumb
        sub r3, r2, #2
        ldr r0, [r3]
        bic r0, r0, #0xFFFFFF00
        b syscall
        bx lr