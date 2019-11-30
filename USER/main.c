#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "exti.h"
#include "tcs3200.h"


 
/************************************************
 S0->PF0      ��ƽ�⣺KEY_UP
 S1->PF1			  ��ʼ��KEY1
 S2->PF2				��ͣ��KEY0
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
	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	EXTIX_Init();
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();
	KEY_Init();
	 
	LCD_Clear(WHITE);
	TCS3200_Init(1,1);//TCS3200��ʼ�������Ƶ�ʶ���100%��
	WhiteBalance();//��ƽ���ʼ��
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
	//ʹ��˵��
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
		key=KEY_Scan(0);   //��������
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
					LCD_ShowString(50, 50,200,20,16,"Running");//����״̬��ʾ
					break;
				case KEY0_PRES:	
					LCD_ShowString(50, 50,200,20,16,"Pausing");//����״̬��ʾ
					temp=0;
					break;
			}
		}
		
//		LCD_ShowxNum(3, 3,temp,1,16,0); 

		if(temp)
		{
			LCD_ShowString(50, 50,200,20,16,"Running");
			R = tcs3200_RED();				//��ȡRGBֵ	
			G = tcs3200_GREEN();	
			B = tcs3200_BLUE();	
		
			RGB_Color = 0x0000;
			RGB_Color |=(unsigned long) R*1000/8225<<11;		//ת������Ļ��16λ��ɫֵ
			RGB_Color |=(unsigned long) G*1000/4047<<5;	  
			RGB_Color |=(unsigned long) B*1000/8225;
		
			/*��������������������������������RGBתHSV�㷨��������������������������������*/
			Rhsv = (float) R/25/10;	//RGBת����0��1.
			Ghsv = (float) G/25/10;	 
			Bhsv = (float) B/25/10;

			Max = (Rhsv>Ghsv)?Rhsv:Ghsv;	
			Max = (Max>Bhsv)?Max:Bhsv;		//ȡRGB���ֵ				  
			Min = (Rhsv<Ghsv)?Rhsv:Ghsv;								   
			Min = (Min<Bhsv)?Min:Bhsv;		//ȥRGB��Сֵ					 
								
			if(Rhsv==Max) H = (Ghsv-Bhsv)/(Max-Min);						  
			if(Ghsv==Max) H = 2+(Bhsv-Rhsv)/(Max-Min);		  					
			if(Bhsv==Max) H = 4+(Rhsv-Ghsv)/(Max-Min);							
			H =(int) (H*60);				//ȡH����ֵ									                  
			if(H<0) H = H+360;													  																		   	   
			V = (Rhsv>Ghsv)?Rhsv:Ghsv;												
			V = (V>Bhsv)?V:Bhsv;		//ȡV����ֵ									 																				  
			S = (Max-Min)/Max;			//ȡS����ֵ																													 	
			V = (int) (V*100);															 
			S = (int) (S*100);
			/*��������������������������������RGBתHSV�㷨��������������������������������*/
		
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



 


