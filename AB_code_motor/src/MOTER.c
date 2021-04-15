#include "moter.h"
#include "timer.h"

void AB_Code_Moter_Init(void){
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//PB0-PB4	AIN2 AIN1 BIN1 BIN2			 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口输出速度
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
    STBY=1;		
}

void STOP()//停
{
	AIN1=0;
	AIN2=0;
	BIN1=1;
	BIN2=1;
}
void GO()//前进
{
	AIN1=0;
	AIN2=1;
    BIN1=0;
	BIN2=1;
}