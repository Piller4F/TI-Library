/****************************************************************************************
@file     infrared.h
@brief    ���������ļ�
@author   ʯ��ǿ
@version  V1.0
@introduction VCC-5V GND->GND D0(������)->PA15 A0(ģ����)->PA0
*****************************************************************************************/

#include "infrared.h"
#include "adc.h"

void HONGWAI_Init(void) {                                   //�����ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;                    //GPIOʵ����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);    //ʹ��GPIOA
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;             //����
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;                //PA15
	GPIO_Init(GPIOA,&GPIO_InitStructure);                   //��ʼ��GPIOA
}

u8   HONGWAI_Check(void) {                    //����ϰ���(1�������ϰ���,0û���ϰ���)
	if(HONGWAI==0) return 1;
	return 0;
}

u16  HONGWAI_Get_Average(u8 ch,u8 times) {    //����ģ������ȡ
	return Get_Adc_Average(ch,times);
}
