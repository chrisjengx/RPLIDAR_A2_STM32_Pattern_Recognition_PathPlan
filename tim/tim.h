#pragma once

#ifdef __cplusplus 
extern "C" {
	#endif  
	
	#include "stm32f10x.h"                  // Device header

	void TIM2_PWM_CH2(u16 ccr2val);

	#ifdef __cplusplus  
} 
#endif 