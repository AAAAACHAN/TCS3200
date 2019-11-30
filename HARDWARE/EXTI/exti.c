#include "exti.h"
#include "delay.h"
#include "stm32f10x_exti.h"
#include "led.h"
#include "tcs3200.h"
  
void EXTIX_Init(void)
{
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
 	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);//PORTF时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//PF4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIOF4 	

 //GPIOF.4	  中断线以及中断初始化配置  下降沿触发	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//使能GPIOF.4所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级0 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 
}

void EXTI4_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位  
	if(amount<999999)
	{
		amount++;
	}
}
 

