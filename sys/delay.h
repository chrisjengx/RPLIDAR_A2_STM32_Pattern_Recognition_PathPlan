#pragma once
	
#ifdef __cplusplus 
extern "C" {
	#endif  
	
	#include "stm32f10x.h"                // Device header
	
	void DelayUs(uint16_t nus);
	void DelayMs(uint16_t nms);

	#ifdef __cplusplus  
} 
#endif 