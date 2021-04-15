#ifndef __MOTER_H
#define __MOTER_H	 
#include "sys.h"


//电机端口定义(PWMA  PB8  PWMB  PB6)  
#define AIN2 PBout(0)
#define AIN1 PBout(1)
#define STBY PBout(2)
#define BIN2 PBout(3)
#define BIN1 PBout(4)	
#define FMQ  PBout(5)
void MOTER_Init(void);	//初始化		 				    
void STOP();//停
void GO();//前进

#endif