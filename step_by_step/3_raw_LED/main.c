/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */
/* main.c */
#define RCC_BASE 0x40021000
#define RCC_CR (*(volatile unsigned long *)(RCC_BASE))
#define RCC_APB2ENR (*(volatile unsigned long *)(RCC_BASE+0x18))

#define GPIOC_BASE 0x40011000
#define GPIOC_CRH   (*(volatile unsigned long *)(GPIOC_BASE+0x04))
#define GPIOC_ODR   (*(volatile unsigned long *)(GPIOC_BASE+0x0c))
int main(){
	int i=1;

	RCC_APB2ENR = 1<<4;  //portC 时钟使能

	GPIOC_CRH = 3<<20;  // pin13 输出模式
	GPIOC_ODR = 0;
	while(1){
		GPIOC_ODR  |= 1<<13;   //pin13 高电平
		for(i=0;i<1000000;i++);
		GPIOC_ODR &= ~(1<<13);  //pin13 低电平
		for(i=0;i<1000000;i++);
	}
	return 0;
}
