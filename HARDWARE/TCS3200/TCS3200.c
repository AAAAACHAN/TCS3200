#include "led.h"
#include "tcs3200.h"
#include "exti.h"
#include "delay.h"

//u16 Rgena=1,Ggena=1,Bgena=1;//用于调节白平衡
u16 Ramount=0,Gamount=0,Bamount=0;//用于存放读取的RGB值
u16 amount=0;//中断计数

_RGB RGB;

/*****************************************************************************
   H指hue（色相）、S指saturation（饱和度）、L指lightness（亮度）、V指value(色调)、B指brightness（明度）。

  色相（H）是色彩的基本属性，就是平常所说的颜色名称，如红色、黄色等。 
  饱和度（S）是指色彩的纯度，越高色彩越纯，低则逐渐变灰，取0-100%的数值。
  明度（V），亮度（B），取0-100%。 	
  
   0度:   红色，RGB:(255, 0,  0 ), 255:R, 0:B，G+　　 
   60度:  黄色，RGB:(255,255, 0 )，255:G, 0:B, R-　　
   120度: 绿色，RGB:( 0, 255, 0 )，255:G, 0:R，B+　　
   180度: 青色，RGB:( 0, 255,255)，255:B, 0:R，G-　　
   240度: 蓝色，RGB:( 0,  0, 255)，255:B, 0:G，R+　　
   300度: 紫色，RGB:(255, 0, 255)，255:R, 0:G，B-　　
   360度: 红色，RGB:(255, 0,  0 )，255:R, 0:B，G+	

  RGB转化到HSV的算法					        
　　　 max=max(R,G,B)
　　　 min=min(R,G,B)
　　   if R = max, H = (G-B)/(max-min)
　　　 if G = max, H = 2 + (B-R)/(max-min)
　　　 if B = max, H = 4 + (R-G)/(max-min)
　　　 H = H * 60
　　　 if H < 0, H = H + 360
　　　 V=max(R,G,B)
　　　 S=(max-min)/max
******************************************************************************/

void TCS3200_Init(u8 s0,u8 s1)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOF, ENABLE);	 
	//S0--->PF0		S1--->PF1		S2--->PF2		S3--->PF3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOF, &GPIO_InitStructure);			

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOF, &GPIO_InitStructure);	

	S0=s0;
	S1=s1;	
}

/*******************************************
*
*		  白平衡
*
*******************************************/
void WhiteBalance(void)
{
	S2=0;S3=0;//红色通道
	amount=0;			 //开始计数
	delay_ms(10);
	RGB.Rgena = amount;   //求出红色因子      
	amount=0;

	S2=1;S3=1;//绿色通道
	amount=0;
	delay_ms(10);
	RGB.Ggena = amount;	 //求出绿色因子
	amount=0;

	S2=0;S3=1;//蓝色通道
	amount=0;
	delay_ms(10);
	RGB.Bgena = amount;	  //求出蓝色因子
	amount=0;
	S2=1;S3=0;//关闭通道  	  
} 

/*******************************************
*
*		  RGB读取
*
*******************************************/
u16 tcs3200_RED(void)
{
	S2=0;S3=0;
	amount=0;
	delay_ms(10);
	S2=1;S3=0;//关闭通道 
	Ramount=(u32) amount*255/RGB.Rgena;	 //取R值
	if(Ramount>255) Ramount = 255;
	return Ramount;
}

u16 tcs3200_GREEN(void)
{
	S2=1;S3=1;
	amount=0;
	delay_ms(10);
	S2=1;S3=0;//关闭通道 
	Gamount=(u32) amount*255/RGB.Ggena;	//取G值
	if(Gamount>255) Gamount = 255;
	return Gamount;
}

u16 tcs3200_BLUE(void)
{
	S2=0;S3=1;
	amount=0;
	delay_ms(10);
	S2=1;S3=0;//关闭通道 
	Bamount=(u32) amount*255/RGB.Bgena;//取B值	
	if(Bamount>255) Bamount = 255;
	return Bamount;
} 

 
