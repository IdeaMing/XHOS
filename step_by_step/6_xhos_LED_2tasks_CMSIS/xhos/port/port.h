#ifndef __PORT_H__
#define __PORT_H__

#include "stm32f10x.h"
#include "xhos.h"
/* 异常向量表映射寄存器 */
#define portCSB_VTOR                         SCB->VTOR

#define portINITIAL_XPSR					( 0x01000000UL )
#define portSTART_ADDRESS_MASK				( ( address_t ) 0xfffffffeUL )

///////////////////////

///////////////////////
void portStartFirstTask( void );
address_t *portInitialiseStack( address_t *TopOfStack, TaskFunction_t pxCode, void *pvParameters );
void portThreadYield(void);
#endif
