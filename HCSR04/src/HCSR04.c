#include "usart.h"
#include "sys.h"
#include "HCSR04.h"
#include "delay.h"
/****************************************************************************************
@file     HCSR04.c
@brief    ���벶��Դ�ļ�
@author   ������
@version  V1.0
@function TIM2 CH1(Echo)->PA0 Trig1 ->PB14
@Notes		���������ͣ�Trig1 ->PB14	����������(Ҳ����TIM2 CH1)��Echo->PA0
					��������ຯ����DIS_Init(u8 *STA,u16 VAL)                     
					������ʹ�ó��������裺1����ʼ��������ģ��HCSR04_Init(arr,psc); 0xFFFF,71
													2����������whileѭ���н�Trig��1
													3����ʱ
													4����Trig��0
													5�����ò�ຯ��
*****************************************************************************************/

u32 DIS_Get()//���������뺯��
{
	u32 temp;
	u32 lenth;
	Trig1=0;
	Trig1=1;
	delay_us(10);
	Trig1=0;
	delay_ms(10);
	  if(TIM2CH1_CAPTURE_STA&0X80)//�ɹ�������һ�θߵ�ƽ
    {
        temp=TIM2CH1_CAPTURE_STA&0X3F;
        temp*=65536;               //���ʱ���ܺ�
        temp+=TIM2CH1_CAPTURE_VAL; //�õ��ܵĸߵ�ƽʱ��
        lenth=temp*0.017;   //���㳤��
        TIM2CH1_CAPTURE_STA=0;     //������һ�β���
    }
	if(lenth>=100) lenth=0;
	return lenth;
}


TIM_ICInitTypeDef  TIM2_ICInitStructure;

void HCSR04_Init()
{	 
	GPIO_InitTypeDef GPIO_InitStructure;														//�����ṹ����
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);						//ʹ��TIM2ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  					//ʹ��GPIOAʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  					//ʹ��GPIOAʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;  										//PA0���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 									//PA0��������  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);								  							//PA0��������
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;  										//PB14���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 								//PB14��������  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 						//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);								  							//PB14�����
	
	//��ʼ����ʱ��2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 												//�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =71; 											//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 				//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  		//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 								//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM2���벶�����
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; 							//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 				 //���������Ƶ,����Ƶ 
  TIM2_ICInitStructure.TIM_ICFilter = 0x01;												 //IC1F=0001 ���������˲��� ���˲�
  TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  									//TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  				//��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  							//�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 									//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  																	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC1,ENABLE);								//��������ж� ,����CC1IE�����ж�	
	
  TIM_Cmd(TIM2,ENABLE ); 																						//ʹ�ܶ�ʱ��2
 
}


u8  TIM2CH1_CAPTURE_STA=0;																					//���벶��״̬		    				
u16	TIM2CH1_CAPTURE_VAL;																						//���벶��ֵ
 
//��ʱ��2�жϷ������	 
void TIM2_IRQHandler(void)
{ 

 	if((TIM2CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{	  
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		 
		{	    
			if(TIM2CH1_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM2CH1_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIM2CH1_CAPTURE_STA|=0X80;//��ǳɹ�������һ��
					TIM2CH1_CAPTURE_VAL=0XFFFF;
				}else TIM2CH1_CAPTURE_STA++;
			}	 
		}
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)//����1���������¼�
		{	
			if(TIM2CH1_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM2CH1_CAPTURE_STA|=0X80;		//��ǳɹ�����һ��������
				TIM2CH1_CAPTURE_VAL=TIM_GetCapture1(TIM2);
		   		TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}else  								//��δ��ʼ,��һ�β���������
			{
				TIM2CH1_CAPTURE_STA=0;			//���
				TIM2CH1_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM2,0);
				TIM2CH1_CAPTURE_STA|=0X40;		//��ǲ�����������
		   		TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
			}		    
		}			     	    					   
 	}
 
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update); 			//����жϱ�־λ
 
}

u32  DIS_Get_Average(u8 times) {
	u8 i;
	u32 sum=0;
	for(i=0;i<times;++i)
		sum+=DIS_Get();
	return sum/times;
}

