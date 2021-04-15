#include "sys.h"
#include "usart.h"		
#include "delay.h"	 
#include "pid.h"
#include "moter.h"
#include "timer.h".


PID s1PID, s2PID;
PID *sptr1 = &s1PID,*sptr2 = &s2PID;

//��ʼ��
void PIDInit(void)
{
	sptr1->iError= 0; //Error[n]
	sptr1->Error1= 0; //Error[n-1]
	sptr1->Proportion= L_Kp; //��������Proportional Const
	sptr1->Integral= L_Ki; //���ֳ���IntegralConst
	sptr1->Derivative=L_Kd; //΢�ֳ���Derivative Const
	
	sptr2->iError= 0; //Error[n]
	sptr2->Error1= 0; //Error[n-1]
	sptr2->Proportion= R_Kp; //��������Proportional Const
	sptr2->Integral= R_Ki; //���ֳ���IntegralConst
	sptr2->Derivative= R_Kd; //΢�ֳ���Derivative Const
}

//����ʽPID
int IncPIDCalc(float CurrentPoint,int SetPoint,PID *sptr)
{
	float  iIncpid; 
	sptr->iError= SetPoint - CurrentPoint; //??
	iIncpid= sptr->Proportion * (sptr->iError-sptr->Error1)                  //p
					+sptr->Integral * sptr->iError                                   //i
					+sptr->Derivative * (sptr->iError-2*sptr->Error1+sptr->Error2);  //d

	//�洢�������´μ���
	sptr->Error2= sptr->Error1;
	sptr->Error1= sptr->iError;
	//��������ֵ
	iIncpid=(int)iIncpid;
	return(iIncpid);
}
int Position_PID (float encoder,float target,PID *sptr)
{   
     float pwm;
		 static float integral_bias;
     sptr->iError=encoder-target;                                        //����ٶ�ƫ��ɲ���ֵ��ȥĿ����?
     integral_bias+=sptr->iError;                                        //���ƫ��Ļ���
	                                                                       //�޷�
     pwm=sptr->Proportion*sptr->iError
	      +sptr->Integral*integral_bias
	      +sptr->Derivative*(sptr->iError-sptr->Error1);                   //λ��ʽpid������
     sptr->Error1=sptr->iError;                                          //������һ��ƫ��
	   pwm=(int)pwm;
     return pwm;                                                          //�������
}

//int Position_PID (int encoder,int target)
//{   
//     static int bias,pwm,integral_bias,last_bias;
//     bias=encoder-target;                                        //����ٶ�ƫ��ɲ���ֵ��ȥĿ����?
//     integral_bias+=bias;                                        //���ƫ��Ļ���
//     pwm=P_Kp*bias+P_Ki*integral_bias+P_Kd*(bias-last_bias);     //λ��ʽpid������
//     last_bias=bias;                                             //������һ��ƫ��
//     return pwm;                                                 //�������
//}

/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
	int a=0,b=0;
	a=xian_fu(moto1);
	PWM1=abs(a);
	b=xian_fu(moto2);
	PWM2=abs(b);
}
int xian_fu(int k)
{
	int a=57599;
	if(k<-a) k=-a;
	if(k>a)  k=a;
	return k;
}
int abs(int a)
{          
      int temp;
      if(a<0)  temp=-a;  
      else temp=a;
      return temp;
}
float Ki_Xian_Fu(float k)
{
	int a=10000;
	if(k<-a) k=-a;
	if(k>a)  k=a;
	return k;
}