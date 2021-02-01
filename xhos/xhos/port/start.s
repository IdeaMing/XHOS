/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */


/* start.S */
.syntax unified /* 统一汇编语法 UAL*/
.cpu cortex-m3
.fpu softvfp
.thumb /* thumb 指令集 */

/* 全局符号 */
.global Vectors

/*****************启动代码**********************/
/* 指定当前段名 */
.section    .text.Reset_Handler
        .weak   Reset_Handler
        .type   Reset_Handler, %function

/* 入口点 */
Reset_Handler:
/* 将数据从 flash copy 到 SRAM */
  movs  r1, #0
  b     LoopCopyDataInit

CopyDataInit:
        ldr     r3, =_sidata
        ldr     r3, [r3, r1]
        str     r3, [r0, r1]
        adds    r1, r1, #4

LoopCopyDataInit:
        ldr     r0, =_sdata
        ldr     r3, =_edata
        adds    r2, r0, r1
        cmp     r2, r3
        bcc     CopyDataInit
        ldr     r2, =_sbss
        b       LoopFillZerobss
/* 用0初始化BSS段 */
FillZerobss:
        movs    r3, #0
        str     r3, [r2], #4

LoopFillZerobss:
        ldr     r3, = _ebss
        cmp     r2, r3
        bcc     FillZerobss
        bl      SystemInit
/* 调用用户实现的main函数 (这就是为什么用户程序以main开始了。当然也可以是别的)*/
        bl      main
        bx      lr
.size   Reset_Handler, .-Reset_Handler



/* 所有数据段的开始，在链接脚本中定义，从这里开始拷贝数据到SRAM */
.word   _sidata
/* .data section的开始地址. 链接脚本中定义 */
.word   _sdata
/* .data section的结束地址. 链接脚本中定义 */
.word   _edata
/* .bss section的开始地址. 链接脚本中定义 */
.word   _sbss
/* .bss section的结束地址. 链接脚本中定义 */
.word   _ebss
/* 栈顶. 链接脚本中定义 */
.word   _estack




/*****************异常向量表**********************/

/* 指定向量表所在的段名 */
.section        .isr_vector,"a",%progbits
.type   Vectors, %object
.size   Vectors, .-Vectors


Vectors:
    .word _estack /*sram 最后*/
    .word Reset_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word 0
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler

