#include "tim.h"

void TIM2_PWM_CH2(u16 ccr2val)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
		/* TIM2_PWM_CH2 PA1 */    
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;   //TX
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;          //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
	

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 199; //Autoreload value ARR
  TIM_TimeBaseStructure.TIM_Prescaler = 71;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	
  TIM_OCInitStructure.TIM_Pulse = ccr2val; /* Set the Capture Compare Register value CCR2 */ 
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
  // Enables or disables the TIMx peripheral Preload register on CCR1.
  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);


	TIM_CtrlPWMOutputs(TIM2,ENABLE);
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}
