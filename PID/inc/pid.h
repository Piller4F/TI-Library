#ifndef _PID_H
#define _PID_H
#include "sys.h"

#define L_Kp 0  //500 //�Ŵ����                 //����ʽPID��KP�൱��λ��ʽ��KD   KI�൱��KP
#define L_Ki 0   //��ʷ����Ӱ��
#define L_Kd 0  //ͻȻ�仯�����Ӱ��

#define R_Kp 0
#define R_Ki 0
#define R_Kd 0

#define P_Kp 0
#define P_Ki 0
#define P_Kd 0
typedef struct PID
{
	//int SetPoint; //Ŀ�� DesiredValue
	double Proportion; //����ϵ��Proportional Const
	double Integral; //���ֳ��� IntegralConst
	double Derivative; //΢�ֳ���Derivative Const
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

