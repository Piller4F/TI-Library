/****************************************************************************************
@file     infrared.h
@brief    红外驱动文件
@author   石国强
@version  V1.0
@introduction VCC-5V GND->GND D0(数字量)->PA15 A0(模拟量)->PA0
*****************************************************************************************/

#include "infrared.h"
#include "adc.h"

void HONGWAI_Init(void) {                                   //红外初始化
	GPIO_InitTypeDef GPIO_InitStructure;                    //GPIO实例化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);    //使能GPIOA
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;             //上拉
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;                //PA15
	GPIO_Init(GPIOA,&GPIO_InitStructure);                   //初始化GPIOA
}

u8   HONGWAI_Check(void) {                    //检测障碍物(1则遇到障碍物,0没有障碍物)
	if(HONGWAI==0) return 1;
	return 0;
}

u16  HONGWAI_Get_Average(u8 ch,u8 times) {    //红外模拟量获取
	return Get_Adc_Average(ch,times);
}
