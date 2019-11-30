#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "exti.h"
#include "tcs3200.h"


 
/************************************************
 S0->PF0      白平衡：KEY_UP
 S1->PF1			  开始：KEY1
 S2->PF2				暂停：KEY0
 S3->PF3
OUT->PF4
************************************************/

 int main(void)
 {	 
	vu8 key=0;
	u8 R=0x00,G=0x00,B=0x00;
	u16 RGB_Color=0x0000;
	int temp=1;
	float Max=0,Min=0;
	float Rhsv=0,Ghsv=0,Bhsv=0;
	float H=0,S=0,V=0;
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	EXTIX_Init();
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			     //LED端口初始化
	LCD_Init();
	KEY_Init();
	 
	LCD_Clear(WHITE);
	TCS3200_Init(1,1);//TCS3200初始化（输出频率定标100%）
	WhiteBalance();//白平衡初始化
	delay_ms(500);
	LCD_ShowString(3, 65,200,20,16,"Rgena:");	
	LCD_ShowString(3, 80,200,20,16,"Ggena:");
	LCD_ShowString(3, 95,200,20,16,"Bgena:");
	LCD_ShowString(3, 110,200,20,16,"R:");	
	LCD_ShowString(3, 125,200,20,16,"G:");
	LCD_ShowString(3, 140,200,20,16,"B:");
	LCD_ShowString(3, 155,200,20,16,"H:");	
	LCD_ShowString(3, 170,200,20,16,"V:");
	LCD_ShowString(3, 185,200,20,16,"S:");
	//使用说明
	LCD_ShowString(3, 5,200,20,16,"WhiteBalance:");
	LCD_ShowString(3, 20,200,20,16,"Run:");
	LCD_ShowString(3, 35,200,20,16,"Pause:");
	LCD_ShowString(3, 50,200,20,16,"State:");

	POINT_COLOR=RED;
	LCD_ShowString(110, 5,200,20,16,"KEY_UP");
	LCD_ShowString(50, 20,200,20,16,"KEY1");
	LCD_ShowString(50, 35,200,20,16,"KEY0");
	LCD_ShowxNum(50, 65,RGB.Rgena,5,16,0); 
	LCD_ShowxNum(50, 80,RGB.Ggena,5,16,0); 
	LCD_ShowxNum(50, 95,RGB.Bgena,5,16,0); 
	
  while(1) 
	{	
		key=KEY_Scan(0);   //按键输入
		if(key)
		{
			switch(key)
			{
				case WKUP_PRES:
					WhiteBalance();
					LCD_ShowxNum(50, 65,RGB.Rgena,5,16,0); 
					LCD_ShowxNum(50, 80,RGB.Ggena,5,16,0); 
					LCD_ShowxNum(50, 95,RGB.Bgena,5,16,0);  
					break;
				case KEY1_PRES:	
					temp=1;
					LCD_ShowString(50, 50,200,20,16,"Running");//运行状态显示
					break;
				case KEY0_PRES:	
					LCD_ShowString(50, 50,200,20,16,"Pausing");//运行状态显示
					temp=0;
					break;
			}
		}
		
//		LCD_ShowxNum(3, 3,temp,1,16,0); 

		if(temp)
		{
			LCD_ShowString(50, 50,200,20,16,"Running");
			R = tcs3200_RED();				//读取RGB值	
			G = tcs3200_GREEN();	
			B = tcs3200_BLUE();	
		
			RGB_Color = 0x0000;
			RGB_Color |=(unsigned long) R*1000/8225<<11;		//转换成屏幕的16位颜色值
			RGB_Color |=(unsigned long) G*1000/4047<<5;	  
			RGB_Color |=(unsigned long) B*1000/8225;
		
			/*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓RGB转HSV算法↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
			Rhsv = (float) R/25/10;	//RGB转换成0，1.
			Ghsv = (float) G/25/10;	 
			Bhsv = (float) B/25/10;

			Max = (Rhsv>Ghsv)?Rhsv:Ghsv;	
			Max = (Max>Bhsv)?Max:Bhsv;		//取RGB最大值				  
			Min = (Rhsv<Ghsv)?Rhsv:Ghsv;								   
			Min = (Min<Bhsv)?Min:Bhsv;		//去RGB最小值					 
								
			if(Rhsv==Max) H = (Ghsv-Bhsv)/(Max-Min);						  
			if(Ghsv==Max) H = 2+(Bhsv-Rhsv)/(Max-Min);		  					
			if(Bhsv==Max) H = 4+(Rhsv-Ghsv)/(Max-Min);							
			H =(int) (H*60);				//取H的数值									                  
			if(H<0) H = H+360;													  																		   	   
			V = (Rhsv>Ghsv)?Rhsv:Ghsv;												
			V = (V>Bhsv)?V:Bhsv;		//取V的数值									 																				  
			S = (Max-Min)/Max;			//取S的数值																													 	
			V = (int) (V*100);															 
			S = (int) (S*100);
			/*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑RGB转HSV算法↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/
		
			LCD_ShowxNum(15, 110,R,3,16,0); 
			LCD_ShowxNum(15, 125,G,3,16,0); 
			LCD_ShowxNum(15, 140,B,3,16,0); 
			LCD_ShowxNum(15, 155,H,3,16,0); 
			LCD_ShowxNum(15, 170,V,3,16,0); 
			LCD_ShowxNum(15, 185,S,3,16,0); 
		
			LCD_Fill(15,210,95,290,RGB_Color);
			delay_ms(50);
		}
	}
}



 


