/****************************************************************************************
@file     infrared.c
@brief    ���������ļ�
@author   ʯ��ǿ
@version  V1.0
@introduction VCC-5V GND->GND D0(������)->PA15 A0(ģ����)->PA0
*****************************************************************************************/

#ifndef __INFRARED_H__
#define __INFRARED_H__

#include "sys.h"

#define HONGWAI PAin(15)

void HONGWAI_Init(void);                     //�����ʼ��
u8   HONGWAI_Check(void);                    //����ϰ���(1�������ϰ���,0û���ϰ���)
u16  HONGWAI_Get_Average(u8 ch,u8 times);    //����ģ������ȡ

#endif
