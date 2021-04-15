/****************************************************************************************
@file     EcodeTIMER.c
@brief    EcodeTIMER源文件
@author   张驰
@version  V1.0
@function TIMER3 CH1->PA6 CH2->PA7 TIMER8 CH1->PC6 CH2->PC7
转速=timer->cnt/（转一圈的所需脉冲数*4）
*****************************************************************************************/


#include "timer.h"
#include "moter.h"
#include "usart.h"

//高级 TIM1 TIM8
//通用 TIM2 TIM3 TIM4 TIM5
//基本 TIM6 TIM7
int N_LV,N_RV,R_V;   //测出的当前的左轮和右轮值(都为正)、平均值
int N_FLV,N_FRV;
//计算电机转速
void speed(){

  int CNT2=TIM3->CNT;
	int CNT1=TIM8->CNT;
	float n_lv=0,n_rv=0,n_flv=0,n_frv=0;
	//printf("PWM1=%d  PWM2=%d\r\n",CNT1,CNT2);
	if(CNT1<=32232){
		n_lv=CNT1*1.0/(xiansu*4)*wheel/T; //???cm/s
		N_LV=(int)n_lv;
		N_FLV=N_LV;
		TIM3->CNT=0;
	}else{
		n_lv=(65535-CNT1)*1.0/(xiansu*4)*wheel/T;
		n_flv=(CNT1-65535)*1.0/(xiansu*4)*wheel/T;
		N_LV=(int)n_lv;
		N_FLV=(int)n_flv;
		TIM3->CNT=0;
	}
	 if(CNT2<=32232){
		n_rv=CNT2*1.0/(xiansu*4)*wheel/T; //???cm/s
		N_RV=(int)n_rv;
		N_FRV=N_RV;
		TIM8->CNT=0;
	}else{
		n_rv=(65535-CNT2)*1.0/(xiansu*4)*wheel/T;
		n_frv=(CNT2-65535)*1.0/(xiansu*4)*wheel/T;
	  N_RV=(int)n_rv;
		N_FRV=(int)n_frv;
		TIM8->CNT=0;
	}
	//printf("N_FLV=%d  N_RV=%d \r\n", N_FLV,N_RV);
	R_V=(N_RV+N_LV)/2.0;
}
 //PWM的输出周期应小于调节速度的周期，而且PWM输出周期应该不应该过慢，否则会导致轮胎卡顿
static void TIM1_TIM4_PWM_Init(u16 arr,u16 psc)  //arr=57599  psc=49  每400ms输出一次PWM  每500ms在中断函数中改变一次
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* TIM1和TIM4 clock enable */
	//72MHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM1|RCC_APB1Periph_TIM4, ENABLE); 

  /* GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

  /*GPIOA Configuration: TIM1 channel 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    //复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

  /*GPIOA Configuration: TIM4 channel 1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    //复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);

 
    /* Time base configuration */		 
  TIM_TimeBaseStructure.TIM_Period = arr-1 ;       
  TIM_TimeBaseStructure.TIM_Prescaler = psc-1;	    
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

  TIM_TimeBaseInit(TIM1|TIM4, &TIM_TimeBaseStructure);
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	   
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = PWM1;	   
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
  TIM_OC4Init(TIM1, &TIM_OCInitStructure);	 //使能通道4
  TIM_CtrlPWMOutputs(TIM1,ENABLE); //MOE 主输出使能
  TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM1, ENABLE);			 //使能预装载
  TIM_OCInitStructure.TIM_Pulse = PWM2;
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);	 //使能通道4
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM4, ENABLE);          //使能预装载

  TIM_Cmd(TIM1|TIM4, ENABLE);                   //使能定时器1和4
}


void TIM3_IRQHandler(void)
{ 		    		  			    
	u16 tsr;
	tsr=TIM3->SR;	
	if(tsr&0X0001)//溢出中断
	{
																				
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位	 
}
void TIM8_IRQHandler(void)
{ 		    		  			    
	u16 tsr;
	tsr=TIM8->SR;	
	if(tsr&0X0001)//溢出中断
	{
																				
	}				   
	TIM8->SR&=~(1<<0);//清除中断标志位	 
}
//定时器3初始化    设置两个定时器为编码器模式接受电机的脉冲 
void TIM3_Encoder_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //TIM3时钟使能
	 
    GPIO_StructInit(&GPIO_InitStructure);
/* 使用PA6和PA7双通道采集脉冲信号 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

/* Enable the TIM3 Update Interrupt */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //优先组级别 用不到
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_DeInit(TIM3);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
    TIM_TimeBaseStructure.TIM_Prescaler = psc-1;  // No prescaling 
    TIM_TimeBaseStructure.TIM_Period = arr-1;  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //时钟分频系数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
 
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge); //双边沿计数
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 6; //无滤波器
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);	  //使能中断

    TIM_Cmd(TIM3, ENABLE);  	   //使能定时器3
}

void TIM8_Encoder_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
/* Encoder unit connected to TIM3, 4X mode */    
  GPIO_InitTypeDef GPIO_InitStructure;
  //NVIC_InitTypeDef NVIC_InitStructure;
  
  /* TIM3 clock source enable */
  RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM8, ENABLE);
  /* Enable GPIOA, clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  /* Configure PA.06,07 as encoder input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* Enable the TIM3 Update Interrupt */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //优先级别
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
   
  /* Timer configuration in Encoder mode */
  TIM_DeInit(TIM8);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
  TIM_TimeBaseStructure.TIM_Prescaler = psc-1;  // No prescaling 
  TIM_TimeBaseStructure.TIM_Period = arr-1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式 
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
 
  TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge); //双边沿计数
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6; //无滤波器
  TIM_ICInit(TIM8, &TIM_ICInitStructure);
  
 // Clear all pending interrupts
 
  TIM_ClearFlag(TIM8, TIM_FLAG_Update);
  TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);	  //使能中断
  
  //Reset counter
  //TIM4->CNT = 0;
  
//  ENC_Clear_Speed_Buffer();
  
  TIM_Cmd(TIM8, ENABLE);  	   //Ê¹ÄÜ¶¨Ê±Æ÷3
}

