#include "sys.h"
#include "usart.h"		
#include "delay.h"	 
#include "pid.h"
#include "moter.h"
#include "timer.h".


PID s1PID, s2PID;
PID *sptr1 = &s1PID,*sptr2 = &s2PID;

//初始化
void PIDInit(void)
{
	sptr1->iError= 0; //Error[n]
	sptr1->Error1= 0; //Error[n-1]
	sptr1->Proportion= L_Kp; //比例常数Proportional Const
	sptr1->Integral= L_Ki; //积分常数IntegralConst
	sptr1->Derivative=L_Kd; //微分常数Derivative Const
	
	sptr2->iError= 0; //Error[n]
	sptr2->Error1= 0; //Error[n-1]
	sptr2->Proportion= R_Kp; //比例常数Proportional Const
	sptr2->Integral= R_Ki; //积分常数IntegralConst
	sptr2->Derivative= R_Kd; //微分常数Derivative Const
}

//增量式PID
int IncPIDCalc(float CurrentPoint,int SetPoint,PID *sptr)
{
	float  iIncpid; 
	sptr->iError= SetPoint - CurrentPoint; //??
	iIncpid= sptr->Proportion * (sptr->iError-sptr->Error1)                  //p
					+sptr->Integral * sptr->iError                                   //i
					+sptr->Derivative * (sptr->iError-2*sptr->Error1+sptr->Error2);  //d

	//存储误差，用于下次计算
	sptr->Error2= sptr->Error1;
	sptr->Error1= sptr->iError;
	//返回增量值
	iIncpid=(int)iIncpid;
	return(iIncpid);
}
int Position_PID (float encoder,float target,PID *sptr)
{   
     float pwm;
		 static float integral_bias;
     sptr->iError=encoder-target;                                        //求出速度偏差，由测量值减去目标手?
     integral_bias+=sptr->iError;                                        //求出偏差的积分
	                                                                       //限幅
     pwm=sptr->Proportion*sptr->iError
	      +sptr->Integral*integral_bias
	      +sptr->Derivative*(sptr->iError-sptr->Error1);                   //位置式pid控制器
     sptr->Error1=sptr->iError;                                          //保存上一次偏差
	   pwm=(int)pwm;
     return pwm;                                                          //增量输出
}

//int Position_PID (int encoder,int target)
//{   
//     static int bias,pwm,integral_bias,last_bias;
//     bias=encoder-target;                                        //求出速度偏差，由测量值减去目标手?
//     integral_bias+=bias;                                        //求出偏差的积分
//     pwm=P_Kp*bias+P_Ki*integral_bias+P_Kd*(bias-last_bias);     //位置式pid控制器
//     last_bias=bias;                                             //保存上一次偏差
//     return pwm;                                                 //增量输出
//}

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：左轮PWM、右轮PWM
返回  值：无
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