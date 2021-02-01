/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */


// XHOS创建闪烁LED task

#include "xhos.h"
#include "stm32f10x.h"
#include "print.h"



void Delay(uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}


/* 定义task1. LED 快闪 */
void task1_func(void)
{

	RCC->APB2ENR |=	RCC_APB2ENR_IOPCEN ;//RCC_APB2ENR = 1<<4;  //portC 时钟使能

	GPIOC->CRH = 3<<20;  // pin13 输出模式
	GPIOC->ODR = 0;
	while(1){
		osPrint("task1 %d\n",g_systick_count);
		GPIOC->ODR  |= 1<<13;   //pin13 高电平
		osmSleep(1000);
		GPIOC->ODR &= ~(1<<13);  //pin13 低电平

	}
}

/* 定义task2. LED 慢闪 */
void task2_func(void)
{

	RCC->APB2ENR |=	RCC_APB2ENR_IOPCEN ;//RCC_APB2ENR = 1<<4;  //portC 时钟使能

	GPIOC->CRH = 3<<20;  // pin13 输出模式
	GPIOC->ODR = 0;
	while(1){
		osPrint("task2 %d\n",g_systick_count);
		GPIOC->ODR  |= 1<<13;   //pin13 高电平
		osmSleep(3000);
		GPIOC->ODR &= ~(1<<13);  //pin13 低电平


	}
}

int main()
{
	// 创建2个task，切换
	osTaskCreate(task2_func);
	osTaskCreate(task1_func);

	// 启动OS调度
	osStartScheduler();

	// 不会到达这里
	while (1);

	return 0;
}
