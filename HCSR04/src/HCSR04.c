#include "usart.h"
#include "sys.h"
#include "HCSR04.h"
#include "delay.h"
/****************************************************************************************
@file     HCSR04.c
@brief    输入捕获源文件
@author   郭汉雄
@version  V1.0
@function TIM2 CH1(Echo)->PA0 Trig1 ->PB14
@Notes		超声波发送：Trig1 ->PB14	超声波接收(也就是TIM2 CH1)：Echo->PA0
					超声波测距函数：DIS_Init(u8 *STA,u16 VAL)                     
					主函数使用超声波步骤：1、初始化超声波模块HCSR04_Init(arr,psc); 0xFFFF,71
													2、在主函数while循环中将Trig置1
													3、延时
													4、将Trig置0
													5、调用测距函数
*****************************************************************************************/

u32 DIS_Get()//定义计算距离函数
{
	u32 temp;
	u32 lenth;
	Trig1=0;
	Trig1=1;
	delay_us(10);
	Trig1=0;
	delay_ms(10);
	  if(TIM2CH1_CAPTURE_STA&0X80)//成功捕获到了一次高电平
    {
        temp=TIM2CH1_CAPTURE_STA&0X3F;
        temp*=65536;               //溢出时间总和
        temp+=TIM2CH1_CAPTURE_VAL; //得到总的高电平时间
        lenth=temp*0.017;   //计算长度
        TIM2CH1_CAPTURE_STA=0;     //开启下一次捕获
    }
	if(lenth>=100) lenth=0;
	return lenth;
}


TIM_ICInitTypeDef  TIM2_ICInitStructure;

void HCSR04_Init()
{	 
	GPIO_InitTypeDef GPIO_InitStructure;														//声明结构体名
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);						//使能TIM2时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  					//使能GPIOA时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  					//使能GPIOA时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;  										//PA0清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 									//PA0设置输入  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);								  							//PA0设置下拉
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;  										//PB14清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 								//PB14设置输入  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 						//IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);								  							//PB14输出高
	
	//初始化定时器2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 												//设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =71; 											//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 				//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  		//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 								//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM2输入捕获参数
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; 							//CC1S=01 	选择输入端 IC1映射到TI1上
  TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 				 //配置输入分频,不分频 
  TIM2_ICInitStructure.TIM_ICFilter = 0x01;												 //IC1F=0001 配置输入滤波器 不滤波
  TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  									//TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  				//先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  							//从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 									//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  																	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC1,ENABLE);								//允许更新中断 ,允许CC1IE捕获中断	
	
  TIM_Cmd(TIM2,ENABLE ); 																						//使能定时器2
 
}


u8  TIM2CH1_CAPTURE_STA=0;																					//输入捕获状态		    				
u16	TIM2CH1_CAPTURE_VAL;																						//输入捕获值
 
//定时器2中断服务程序	 
void TIM2_IRQHandler(void)
{ 

 	if((TIM2CH1_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{	  
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		 
		{	    
			if(TIM2CH1_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIM2CH1_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					TIM2CH1_CAPTURE_STA|=0X80;//标记成功捕获了一次
					TIM2CH1_CAPTURE_VAL=0XFFFF;
				}else TIM2CH1_CAPTURE_STA++;
			}	 
		}
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
		{	
			if(TIM2CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM2CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次上升沿
				TIM2CH1_CAPTURE_VAL=TIM_GetCapture1(TIM2);
		   		TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
			}else  								//还未开始,第一次捕获上升沿
			{
				TIM2CH1_CAPTURE_STA=0;			//清空
				TIM2CH1_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM2,0);
				TIM2CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
		   		TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
			}		    
		}			     	    					   
 	}
 
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update); 			//清除中断标志位
 
}

u32  DIS_Get_Average(u8 times) {
	u8 i;
	u32 sum=0;
	for(i=0;i<times;++i)
		sum+=DIS_Get();
	return sum/times;
}

