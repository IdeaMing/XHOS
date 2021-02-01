/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */


#include "XHOS.h"


/* 异常向量表映射寄存器 */
#define devCSB_VTOR                     ( * ( ( volatile uint32_t * ) 0xE000ED08 ) )

#define devINITIAL_XPSR					( 0x01000000UL )
#define devSTART_ADDRESS_MASK			( ( address_t ) 0xfffffffeUL )

/* 触发PendSV */
#define devNVIC_INT_CTRL_REG            ( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
#define devNVIC_PENDSVSET_BIT           ( 1UL << 28UL )

TCB_t * g_Current;
int g_task_count = 0;
extern void test_err();
TCB_t tasktcb[2];

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

void __attribute__ (( naked ))  SVC_Handler( void )
{
	__asm volatile 
	(
	"       ldr r3, CurrentTCB          \n" /* 获取task的TCB */
	"       ldr r1, [r3]                \n"
	"       ldr r0, [r1]                \n" /* TCB 中第一个就是栈顶 */
	"       ldmia r0!, {r4-r11}         \n" /* 将栈中保存的寄存器值恢复到寄存器 */
	"       msr psp, r0                 \n" /* PSP指向用户task的栈顶 */
	"       isb                         \n"
	"       mov r0, #0                  \n"
	"       msr     basepri, r0         \n"
	"       orr r14, #0xd               \n" /* 用于使CPU返回用户模式 */
	"       bx r14                      \n" /* 异常返回 */
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
	__asm volatile
	(
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

/* 2个task切换 */
void getNextTask(){
	int pid = g_Current->pid;
	int next = (pid+1) %2;
	g_Current = &tasktcb[next];
}

/* 这是一个裸函数(naked function)，编译器不生成其他代码. */
void __attribute__ (( naked )) PendSV_Handler( void )
{

        __asm volatile
        (
        "       mrs r0, psp                     \n"
        "       isb                             \n"
        "                                       \n"
        "       ldr     r3, =g_Current          \n" /* r3保存g_Current的地址（指向g_Current） */
        "       ldr     r2, [r3]                \n" /* r2保存TCB第一个元素地址 （指向TopOfStack） */
        "                                       \n"
        "       stmdb r0!, {r4-r11}             \n" /* 寄存器r4-r11保存在psp */
        "       str r0, [r2]                    \n" /* Save the new top of stack into the first member of the TCB. */
        "                                       \n"
        "       stmdb sp!, {r3, r14}            \n" /* 临时保存r3 r14 */
        "       mov r0, #0                      \n"
        "       msr basepri, r0                 \n"
        "       bl getNextTask                  \n" /* g_Current 指向下一个task， */
        "       mov r0, #0                      \n"
        "       msr basepri, r0                 \n"
        "       ldmia sp!, {r3, r14}            \n"
        "                                       \n"     /* Restore the context, including the critical nesting count. */
        "       ldr r1, [r3]                    \n" /* 此时r3还是指向g_Current, 但是g_Current 已经指向了新的task*/
        "       ldr r0, [r1]                    \n" /* The first item in pxCurrentTCB is the task top of stack. */
        "       ldmia r0!, {r4-r11}             \n" /* Pop the registers. */
        "       msr psp, r0                     \n"
        "       isb                             \n"
        "       bx r14                          \n"
        "                                       \n"
        "       .align 4                        \n"
        "       .word g_Current                 \n"
        ::
        );
}

void osThreadYield() 
{                                                                                 
    /* 写NVIC寄存器触发PendSV异常 */                           
    devNVIC_INT_CTRL_REG = devNVIC_PENDSVSET_BIT;                             
                                                                                  
	/* 保险起见使用屏障指令同步 */                   
    __asm volatile( "dsb" );                                                  
    __asm volatile( "isb" );                                                   
}


// 创建task
void osTaskCreate(TaskFunction_t task_func){
	int pid = g_task_count++;
	g_Current = &tasktcb[pid];
	g_Current->pid = pid;
	g_Current->TopOfStack = devInitialiseStack(g_Current->stack+conf_STACK_SIZE, task_func, NULL);

}

// 启动OS调度
void osStartScheduler(){

	/* Start the first task. */
	StartFirstTask();

	/* Should not get here! */
	return ;
}
