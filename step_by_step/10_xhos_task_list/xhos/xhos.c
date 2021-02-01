/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */

#include "xhos.h"
#include "port.h"
#include "print.h"

// 如果使用BSS段malloc，局变量必须在BSS段，也就是初始化为0 否则有莫名其妙的错误，why?

TCB_t * g_Current;
int g_systick_count = 0;
extern void test_err();


int g_task_count = 1;

void os_Prepare(){
	list_init(&g_taskReadyList);

}

void os_DealSysTick(){
	g_systick_count++;
}

/* 2个task切换 */
void os_getNextTask(){
	__disable_irq();
	list_del(&g_Current->task_node);
	list_add_tail(&g_Current->task_node, &g_taskReadyList);
	NODE *root = &g_taskReadyList;
	NODE *node = root->next;

	g_Current = node2Task(node);
	__enable_irq();

}



void osThreadYield() 
{                                                                                 
	portThreadYield();
}

// 创建task
void osTaskCreate(TaskFunction_t task_func){
	int pid = g_task_count++;
	g_Current = osMalloc(sizeof(TCB_t));
	g_Current->pid = pid;
	osPrint("osTaskCreate %d\n",pid);
	list_add(&g_Current->task_node, &g_taskReadyList);
	g_Current->TopOfStack = portInitialiseStack((address_t *)(g_Current->stack+conf_STACK_SIZE-1), task_func, NULL);


}

// 启动OS调度
void osStartScheduler(){

        /* Start the first task. */
        portStartFirstTask();

        /* Should not get here! */
        return ;
}

void * osMalloc(size_t size){
	return  portMalloc(size);
}
