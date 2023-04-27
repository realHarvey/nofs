/*
 * 链接脚本
 * Copyright(c) 2023/01  Harvey
 */
MEMORY
{
    ram  (rwx) : org = 0x20000000, len = 20k
    flash(rwx) : org = 0x08000000, len = 64k
}

msp_init = ORIGIN(ram) + LENGTH(ram);

SECTIONS
{
    /* ======= Vector ========
     * 中断向量表
     */
    .vector : { KEEP( *(.vector) ) } > flash

    /* ======== Code =========
     */
    .text : {
        *(.text .text.*)
    } > flash

    /* ======= roData ========
     */
    .rodata : {
        . = ALIGN(4);
        *(.rodata .rodata.*)

        . = ALIGN(4);
        PROVIDE( endof_rodata = . );
    } > flash

    /* ========= Data =========
     * 装载到紧邻rodata的flash区域, 启动时拷贝到ram
     * flash_addr_data : .data段装载的地址
     */
    flash_addr_data = LOADADDR(.data);
    .data : AT(endof_rodata) {
        PROVIDE( startof_data = . );
        *(.data .data.*)
        PROVIDE( endof_data = . );
    } > ram

    /* ========= BSS =========
     * boot.s中对bss段0初始化
     */
    .bss : {
        . = ALIGN(4);
        PROVIDE( startof_bss = . );
        *(.bss .bss.* COMMON)
        PROVIDE( endof_bss = . );
    } > ram

    /* 抛弃的libs */
    /DISCARD/ : { libc.a ( * ) libm.a ( * ) libgcc.a ( * ) }
}