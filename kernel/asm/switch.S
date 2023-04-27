.syntax unified
.text

.globl  task_switch

/*
 * 任务切换
 * void task_switch(struct task *old, struct task *new);
 */
task_switch :
/* keep old context to pcb */
        sub r0, r0, #24
        stmdb r0!, {r4-r11}
        // 不能合并push, 汇编器会纠序
        mrs r2, psp
        stmdb r0!, {r2}

/* load from next pcb */
        sub r1, r1, #60
        ldmia r1!, {r2}
        msr psp, r2
        ldmia r1!, {r4-r11}

        bx lr