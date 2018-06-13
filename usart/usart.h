#pragma once

#ifdef __cplusplus 
extern "C" {
	#endif  
	
	#include "stm32f10x.h"                  // Device header

	void usart(USART_TypeDef *USARTx,uint16_t USART_IT_x,FunctionalState STATE);
	void USART_NVIC_Configuration();
	void USART1_IRQHandler();
	void USART2_IRQHandler();
	
	#ifdef __cplusplus  
} 
#endif 