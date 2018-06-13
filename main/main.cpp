#include "stm32f10x.h"                  // Device header
#include "sysclk.h"
#include "usart.h"
#include "tim.h"
#include "delay.h"


int main()
{
	SystemClockConfiguration();
	usart(USART1, USART_IT_RXNE,ENABLE);
	usart(USART2, USART_IT_RXNE,ENABLE);
	USART_NVIC_Configuration();
	
	DelayMs(1000);


	while(1) 
  {}
	
	return 0;
}