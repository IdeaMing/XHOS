#include "port.h"
#include "xhos.h"


address_t *portInitialiseStack( address_t *TopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
        TopOfStack = (address_t)TopOfStack & (~0x0000000f); // 双字栈对齐
	/* 制造现场，好像该任务是被切换除去，用同样的方法可以切入 */
	TopOfStack--;
	*TopOfStack = portINITIAL_XPSR;       /* xPSR */
	TopOfStack--;
	*TopOfStack = ( ( address_t ) pxCode ) & portSTART_ADDRESS_MASK;    /* PC */
	TopOfStack--;
	*TopOfStack = ( address_t ) 0;        /* LR */
	TopOfStack -= 5;      /* R12, R3, R2 and R1. */
	*TopOfStack = ( address_t ) pvParameters;   /* R0 */
	TopOfStack -= 8;      /* R11, R10, R9, R8, R7, R6, R5 and R4. */

	return TopOfStack;
}

void SystemInit(){

        /* 使能双字栈对齐 */
        SCB->CCR |= SCB_CCR_STKALIGN_Msk;

	/*为OS启动做准备*/
	os_Prepare();

	/* 开启HSI 即内部晶振时钟 */
	RCC->CR |= RCC_CR_HSION; 

	/* 选择HSI为PLL的时钟源HSI必须2分频给PLL */
	RCC->CFGR |= (uint32_t)RCC_CFGR_PLLSRC_HSI_Div2; 

	/*PLLCLK=8/2*9=36MHz 设置倍频得到时钟源PLL的频率 */
	RCC->CFGR |= (uint32_t)RCC_CFGR_PLLMULL9;

	/* PLL不分频输出  */
	RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

	/* 使能 PLL时钟 */
	RCC->CR |= RCC_CR_PLLON;

	/* 等待PLL时钟就绪 */
	while((RCC->CR & RCC_CR_PLLRDY) == 0){}

	/* 选择PLL为系统时钟的时钟源 */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

	/* 等到PLL成为系统时钟的时钟源 */
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08){ }

	/* 当前APB2/pclk2 是36MHz,设置波特率115200 */
	console_init(36,115200);


        /* Configure SysTick to interrupt at the requested rate. */
	SysTick_Config(36000000/1000); // 1秒1000次中断
        //portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
        //portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT );

        /* Make PendSV and SysTick the lowest priority interrupts. */
	NVIC_SetPriority(PendSV_IRQn,0xff);
	NVIC_SetPriority(SysTick_IRQn,0xff);
        //portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
        //portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;


}
void portThreadYield(){

        /* 写NVIC寄存器触发PendSV异常 */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

	/* 保险起见使用屏障指令同步 */
        __asm volatile( "dsb" );
        __asm volatile( "isb" );
}

void SysTick_Handler(void){

	os_DealSysTick();

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

void portStartFirstTask( void )
{
	// 重定位中断向量
	portCSB_VTOR = 0x08000000;
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
        "       mov r0, #0x10                   \n" /* configMAX_SYSCALL_INTERRUPT_PRIORITY */
        "       msr basepri, r0                 \n"
        "       bl os_getNextTask               \n" /* g_Current 指向下一个task， */
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
//初始化 串口1，其他类似
//pclk2:PCLK2 时钟频率(Mhz)
//bound:波特率
void console_init(uint32_t pclk2,uint32_t bound)
{
        float temp;
        uint16_t mantissa;
        uint16_t fraction;
        temp=(float)(pclk2*1000000)/(bound*16);//得到 USARTDIV
        mantissa=temp;
        //得到整数部分
        fraction=(temp-mantissa)*16; //得到小数部分
        mantissa<<=4;
        mantissa+=fraction;
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
        //使能 PORTA 口时钟
        RCC->APB2ENR|=RCC_APB2ENR_USART1EN; //使能串口时钟
        GPIOA->CRH&=0XFFFFF00F;//IO 状态设置
        GPIOA->CRH|=0X000008B0;//IO 状态设置
        RCC->APB2RSTR|=RCC_APB2RSTR_USART1RST; //复位串口 1
        RCC->APB2RSTR&=~(RCC_APB2RSTR_USART1RST);//停止复位
        //波特率设置
        USART1->BRR=mantissa; // 波特率设置
        USART1->CR1|=0X200C; //1 位停止,无校验位.

}

void portPrintChar(char **str, unsigned int c)
{
	USART1->DR=c;
	while((USART1->SR&0X40)==0);//等待发送结束
}
