#ifndef _TIMER_H
#define _TIMER_H


#include "sys.h"

#define  wheel 21.5
#define  xiansu 135

#define PWM1 TIM1->CCR4  
#define PWM2 TIM4->CCR1

#define T  0.75//1//0.5  //((TIM2->ARR+1)*(TIM2->PSC+1))/72000000  //采样周期
void speed();

static void TIM1_TIM4_PWM_Init(u16 arr,u16 psc);
void TIM3_Encoder_Init(u16 arr,u16 psc);
void TIM8_Encoder_Init(u16 arr,u16 psc);

#endif