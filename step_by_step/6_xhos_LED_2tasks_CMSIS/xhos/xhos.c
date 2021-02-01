/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */

#include "xhos.h"
#include "port.h"


TCB_t * g_Current;
extern void test_err();
TCB_t tasktcb[2];
int g_task_count = 0;



/* 2个task切换 */
void os_getNextTask(){
	int pid = g_Current->pid;
	int next = (pid+1) %2;
	g_Current = &tasktcb[next];
}



void osThreadYield() 
{                                                                                 
	portThreadYield();
}
// 创建task
void osTaskCreate(TaskFunction_t task_func){
        int pid = g_task_count++;
        g_Current = &tasktcb[pid];
        g_Current->pid = pid;
        g_Current->TopOfStack = portInitialiseStack(g_Current->stack+conf_STACK_SIZE, task_func, NULL);


}

// 启动OS调度
void osStartScheduler(){

        /* Start the first task. */
        portStartFirstTask();

        /* Should not get here! */
        return ;
}

