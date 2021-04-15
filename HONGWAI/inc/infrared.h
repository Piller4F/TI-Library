/****************************************************************************************
@file     infrared.c
@brief    红外驱动文件
@author   石国强
@version  V1.0
@introduction VCC-5V GND->GND D0(数字量)->PA15 A0(模拟量)->PA0
*****************************************************************************************/

#ifndef __INFRARED_H__
#define __INFRARED_H__

#include "sys.h"

#define HONGWAI PAin(15)

void HONGWAI_Init(void);                     //红外初始化
u8   HONGWAI_Check(void);                    //检测障碍物(1则遇到障碍物,0没有障碍物)
u16  HONGWAI_Get_Average(u8 ch,u8 times);    //红外模拟量获取

#endif
