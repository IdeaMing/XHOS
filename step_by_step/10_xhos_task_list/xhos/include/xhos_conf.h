/*
 * XHOS 一个为了学习而生的OS
 *  官方网站：findxiaohei.xyz
 */

#ifndef XHOS_CONF_H
#define XHOS_CONF_H

/*
 * XHOS的头文件，使用XHOS必须包含该文件
 */

#ifdef __cplusplus
extern "C" {
#endif
///////////////////////
#define conf_STACK_SIZE 512

// HEAP大小为BSS结束到主栈结束，假设主栈1024字节。
//#define conf_HEAP_SIZE 10240
#define conf_MAIN_STACK_SIZE 1024


//////////////////////
#ifdef __cplusplus
}
#endif

#endif


