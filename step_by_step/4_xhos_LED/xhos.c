/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */


#include "XHOS.h"



#define devCSB_VTOR                     ( * ( ( volatile uint32_t * ) 0xE000ED08 ) )

#define devINITIAL_XPSR			( 0x01000000UL )
#define devSTART_ADDRESS_MASK		( ( address_t ) 0xfffffffeUL )

TCB_t * g_Current;
extern void test_err();
TCB_t task1tcb;

address_t *devInitialiseStack( address_t *TopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* 制造现场，好像该任务是被切换除去，用同样的方法可以切入 */
	TopOfStack--;
	*TopOfStack = devINITIAL_XPSR;       /* xPSR */
	TopOfStack--;
	*TopOfStack = ( ( address_t ) pxCode ) & devSTART_ADDRESS_MASK;    /* PC */
	TopOfStack--;
	*TopOfStack = ( address_t ) 0;        /* LR */
	TopOfStack -= 5;      /* R12, R3, R2 and R1. */
	*TopOfStack = ( address_t ) pvParameters;   /* R0 */
	TopOfStack -= 8;      /* R11, R10, R9, R8, R7, R6, R5 and R4. */

	return TopOfStack;
}

void SVC_Handler( void )
{
	__asm volatile (
			"       ldr r3, CurrentTCB          \n" /* 获取task的TCB */
			"       ldr r1, [r3]                \n"
			"       ldr r0, [r1]                \n" /* TCB 中第一个就是栈顶 */
			"       ldmia r0!, {r4-r11}         \n" /* 将栈中保存的寄存器值恢复到寄存器 */
			"       msr psp, r0                 \n" /* PSP指向用户task的栈顶 */
			"       isb                         \n"
			"       mov r0, #0                  \n"
			"       msr     basepri, r0         \n"
			"       orr r14, #0xd               \n" /* 用于使CPU返回用户模式 */
			"       bx r14                      \n"
			"                                   \n"
			"       .align 4                    \n"
			"CurrentTCB: .word g_Current          \n"
		       );
}
/*-----------------------------------------------------------*/

static void StartFirstTask( void )
{
	// 重定位中断向量
	devCSB_VTOR = 0x08000000;
	__asm volatile(
			" ldr r0, =0xE000ED08      \n" /* 从NVIC 的VTOR中获取中断向量表，向量表的第一个就是栈顶 */
			" ldr r0, [r0]             \n"
			" ldr r0, [r0]             \n" 
			" msr msp, r0              \n" /* 将msp设置为栈顶 */
			" cpsie i                  \n" /* 打开中断 */
			" cpsie f                  \n"
			" dsb                      \n"
			" isb                      \n" 
			" svc 0                    \n" /* 使用SVC启动第一个task */
			" nop                      \n"
		      );
}


// 创建task
void osTaskCreate(TaskFunction_t task_func){
	g_Current = &task1tcb;
	g_Current->TopOfStack = devInitialiseStack(g_Current->stack+conf_STACK_SIZE, task_func, NULL);

}

// 启动OS调度
void osStartScheduler(){

	/* Start the first task. */
	StartFirstTask();

	/* Should not get here! */
	return ;
}
