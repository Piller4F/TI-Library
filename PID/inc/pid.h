#ifndef _PID_H
#define _PID_H
#include "sys.h"

#define L_Kp 0  //500 //放大比例                 //增量式PID的KP相当于位置式的KD   KI相当于KP
#define L_Ki 0   //历史因素影响
#define L_Kd 0  //突然变化过多会影响

#define R_Kp 0
#define R_Ki 0
#define R_Kd 0

#define P_Kp 0
#define P_Ki 0
#define P_Kd 0
typedef struct PID
{
	//int SetPoint; //目标 DesiredValue
	double Proportion; //比例系数Proportional Const
	double Integral; //积分常数 IntegralConst
	double Derivative; //微分常数Derivative Const
	float Error1; //Error[n-1]
	float iError; //Error[n]
} PID;

//int Incremental_PI (int Encoder,int Target);

void PIDInit(void);
int IncPIDCalc(float CurrentPoint,int SetPoint,PID *sptr);
int Position_PID (float Encoder,float Target,PID *sptr);
void Set_Pwm(int moto1,int moto2);
int abs(int a);
int xian_fu(int k);
float Ki_Xian_Fu(float k);
#endif

