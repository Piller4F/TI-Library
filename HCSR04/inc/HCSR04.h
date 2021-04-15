#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "mymath.h"

#define Trig1 PBout(14)

extern u8  TIM2CH1_CAPTURE_STA;																					// ‰»Î≤∂ªÒ◊¥Ã¨		    				
extern u16	TIM2CH1_CAPTURE_VAL;	

void HCSR04_Init(void);
u32  DIS_Get(void);
u32  DIS_Get_Average(u8 times);

#endif
