/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */

#ifndef XHOS_H
#define XHOS_H

/*
 * XHOS的头文件，使用XHOS必须包含该文件
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "xhos_conf.h"
/////////////////////////////                                                          

////////////////////////////                                                           
typedef uint32_t address_t;
typedef long Base_t;
typedef unsigned long UBase_t;
typedef uint32_t size_t;
#define NULL 0


// 定义函数指针，用于任务创建。
typedef void (*TaskFunction_t)( void );

// 任务控制块，就是传说中的TCB
typedef struct TaskControlBlock
{
	// 第一个必须是栈顶！！！
        volatile address_t    *TopOfStack;

        // 任务栈                                                                      
        char stack[conf_STACK_SIZE];    
       	Base_t pid;
}TCB_t;


//////////////////////////////////////////
TCB_t g_idleTask;


extern int g_systick_count;

void os_DealSysTick();

///////////////////////////////////////////

// 创建task
void osTaskCreate(TaskFunction_t task_func);

// 启动OS调度
void osStartScheduler();

// 主动放弃CPU
void osThreadYield();


void * osMalloc(size_t size);
#ifdef __cplusplus
}
#endif

#endif


