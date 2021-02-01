#include "xhos.h"
#include "xhos_conf.h"
#include "print.h"
#if 1

/*链接脚本指定_heap_base和大小
    ._heap :
    {
    . = ALIGN(32); 
    _heap_base = .;   // &_heap_base 就是开始地址
    . = . + 10240;
    } >ram
*/
// 除了data和BSS段，主栈1024字节，剩余都做堆
//#define conf_HEAP_SIZE 10240
extern address_t _heap_base;
extern address_t _estack;
static address_t g_heap = 0;
static address_t g_heap_end = 0;

/* 内存不足返回0 */
void * portMalloc(size_t size){
	if(g_heap == 0){
		g_heap = &_heap_base;
		g_heap_end = (char*)(&_estack) - conf_MAIN_STACK_SIZE;
		// osPrint("g_heap_end %x = &_estack %x - conf_MAIN_STACK_SIZE %x\n",g_heap_end , &_estack , conf_MAIN_STACK_SIZE);
	}
	g_heap = ( ( address_t ) g_heap+8) & ( ~( ( address_t ) 0x0007 ) );
	void *p = NULL;
	// 8 Bytes aligment
	size += 8-size%8;
	
	if(g_heap+size > g_heap_end){
		osPrint("no memory\n");
		return NULL;
	}
	p = (void *)g_heap;
	g_heap += size;
	return p;
}



#else
// 大数组自己指定大小
#define conf_HEAP_SIZE 10240
static uint8_t _heap_base[conf_HEAP_SIZE] = {0}; // 链接脚本中定义_heap_base

//extern address_t _heap_base;
static address_t g_heap = 0;
static address_t g_heap_end = 0;


/* 内存不足返回0 */
void * portMalloc(size_t size){
	if(g_heap == 0){
		g_heap = (address_t)_heap_base;
		g_heap_end = g_heap + conf_HEAP_SIZE;
	}
	g_heap = ( ( address_t ) g_heap+8) & ( ~( ( address_t ) 0x000f ) );
	void *p = NULL;
	// 8 Bytes aligment
	size += 8-size%8;
	
	if(g_heap+size > g_heap_end){
		// no memory
		return NULL;
	}
	p = (void *)g_heap;
	g_heap += size;
	return p;
}

#endif
