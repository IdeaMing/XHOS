/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */

#include "xhos.h"
#include "port.h"
#include "print.h"

// 如果使用BSS段malloc，局变量必须在BSS段，也就是初始化为0 否则有莫名其妙的错误，why?
int g_task_count = 1;
TCB_t * g_Current;
int g_systick_count = 0;
extern void test_err();




void os_Prepare(){
	list_init(&g_taskReadyList);
	list_init(&g_taskBlockedList);

	//初始化idle task,当没有任务时（sleep）执行idle
	g_idleTask.pid=0;
	g_idleTask.state = taskReady;
	list_init(&g_idleTask.task_node);
	//list_add(&g_idleTask.task_node, &g_taskReadyList);
	g_idleTask.TopOfStack = (address_t *)portInitialiseStack((address_t *)(g_idleTask.stack+conf_STACK_SIZE-1), portIdleTask, NULL);

}

void os_DealSysTick(){

	g_systick_count++;
	
	// sleep的task睡眠时间减小到0则投入运行。
	// 可以优化，为了代码简单而为之
	int should_switch = 0;
	TCB_t *t=NULL;
	NODE *root = &g_taskBlockedList;
	NODE *node = root->next;
	while(root!=node){
		t = node2Task(node);
		node = node->next;
		//list_del(node);
		t->sleep_count--;
		if(t->sleep_count <= 0){
			should_switch = 1;
			t->state = taskReady;
			list_del(&t->task_node);
			list_add(&t->task_node, &g_taskReadyList);
		}
	}

        if(should_switch)osThreadYield();                          
}

void list_debug(NODE * root){

	NODE *node = root->next;
	if(root!=node){
		TCB_t * t = node2Task(node);
		node = node->next;
		osPrint("pid = %d\n",t->pid);
		//list_del(node);
	}

}

/* 从ready队列取出第一个task执行，空队列则运行idle*/
TCB_t * get_next_task(){
	TCB_t *ret = &g_idleTask;
	NODE *root = &g_taskReadyList;
	NODE *node = root->next;
	if(root!=node){
		ret = node2Task(node);
		//list_del(node);
	}

	return ret;
}


void os_getNextTask(){
	__disable_irq();
		list_del(&g_Current->task_node);
		if(g_Current->state == taskBlocked){
			list_add(&g_Current->task_node, &g_taskBlockedList);
		}else{
			if(g_Current->pid != 0)
				list_add_tail(&g_Current->task_node, &g_taskReadyList);
		}

	g_Current = get_next_task();
	__enable_irq();

}


void osmSleep(Base_t n){

	g_Current->sleep_count = n;
	g_Current->state = taskBlocked;
//	osPrint("sleep %d\n", g_Current->pid);
	//list_del(&g_Current->task_node);
	//list_add_tail(&g_Current->task_node, &g_taskBlockedList);
	//g_Current = get_next_task();
	osThreadYield();
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
