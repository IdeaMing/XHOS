/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */

/* start.S */

.text
.globl _start 
 
_start:
    .word 0x20000000+20480 /*sram form 0x20000000,size 20K*/
    .word main /*跳转到main函数*/
