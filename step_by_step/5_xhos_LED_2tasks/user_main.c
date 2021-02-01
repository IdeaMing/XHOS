/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */


// XHOS创建闪烁LED task

#include "XHOS.h"

/* 方便C语言操作寄存器 */
#define RCC_BASE 0x40021000
#define RCC_CR (*(volatile unsigned long *)(RCC_BASE))
#define RCC_APB2ENR (*(volatile unsigned long *)(RCC_BASE+0x18))

#define GPIOC_BASE 0x40011000
#define GPIOC_CRH   (*(volatile unsigned long *)(GPIOC_BASE+0x04))
#define GPIOC_ODR   (*(volatile unsigned long *)(GPIOC_BASE+0x0c))


void Delay(uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}


/* 定义task1. LED 快闪 */
void task1_func(void)
{
	int ii=0;

	RCC_APB2ENR = 1<<4;  //portC 时钟使能

	GPIOC_CRH = 3<<20;  // pin13 输出模式
	GPIOC_ODR = 0;
	while(1){
		GPIOC_ODR  |= 1<<13;   //pin13 高电平
		Delay(100000);
		GPIOC_ODR &= ~(1<<13);  //pin13 低电平
		Delay(100000);

		// 自动放弃CPU
		osThreadYield();
	}
}

/* 定义task2. LED 慢闪 */
void task2_func(void)
{
	int i=0;

	RCC_APB2ENR = 1<<4;  //portC 时钟使能

	GPIOC_CRH = 3<<20;  // pin13 输出模式
	GPIOC_ODR = 0;
	while(1){
		GPIOC_ODR  |= 1<<13;   //pin13 高电平
		Delay(1000000); // 延时增加
		GPIOC_ODR &= ~(1<<13);  //pin13 低电平
		Delay(1000000);
		
		// 自动放弃CPU
		osThreadYield();
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
