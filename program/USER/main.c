#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "wifi.h"
#include "dht11.h"
#include "lcd1602.h"
#include "adc.h"
#include "timer.h"

u8 bbuf[16]="WIFI ERROE!     ";
char bbbuf[12]="SMOG HIGH!!!";
u8 buf[6]="Auto  ";
u8 buf1[6]="Manual";
u8 humi_value,temp_value;
u8 flag,flag1;
u16 smog_value,beam_value;

//用于数据传输
//0-3:温度值
//4-7:烟雾值
//8-11：光照值
char value_buf[14];  


//正转
void F_Rotation(u16 x)
{
	u8 j;
	for(;x>0;x--)
	{
		switch(j)
		{
			case 0:motor1 = 1;motor2 = 0;motor3 = 0;motor4 = 0;break;
			case 1:motor1 = 0;motor2 = 1;motor3 = 0;motor4 = 0;break;
			case 2:motor1 = 0;motor2 = 0;motor3 = 1;motor4 = 0;break;
			case 3:motor1 = 0;motor2 = 0;motor3 = 0;motor4 = 1;break;
		}
		j++;
		if(j>=4)   j=0;
		delay_ms(2);
	}
	
}

//反转
void B_Rotation(u16 x)
{
	u8 j;
	for(;x>0;x--)
	{
		switch(j)
		{
			case 0:motor1 = 0;motor2 = 0;motor3 = 0;motor4 = 1;break;
			case 1:motor1 = 0;motor2 = 0;motor3 = 1;motor4 = 0;break;
			case 2:motor1 = 0;motor2 = 1;motor3 = 0;motor4 = 0;break;
			case 3:motor1 = 1;motor2 = 0;motor3 = 0;motor4 = 0;break;
		}
		j++;
		if(j>=4)   j=0;
		delay_ms(2);
	}	
}

//数据处理，显示、判断
void Data_Deal(void)
{
	DHT11_Read_Data(&temp_value,&humi_value);  //读取温湿度值
	smog_value = Get_Adc_Average(ADC_Channel_9,20);  //读取烟雾浓度值
	beam_value = Get_Adc_Average(ADC_Channel_8,20);  //读取光照值
	
	//LCD显示数据
	//显示温度
	value_buf[0] = 0x30 + temp_value / 1000;
	value_buf[1] = 0x30 + temp_value % 1000 / 100;
	value_buf[2] = 0x30 + temp_value % 100 / 10;
	value_buf[3] = 0x30 + temp_value % 10;
	LCD1602_Write_Cmd(0x80);
	LCD1602_Write_Dat('T');
	LCD1602_Write_Dat(':');
	LCD1602_Write_Dat(value_buf[0]);
	LCD1602_Write_Dat(value_buf[1]);
	LCD1602_Write_Dat(value_buf[2]);
	LCD1602_Write_Dat(value_buf[3]);
	
	
	value_buf[4] = 0x30 + smog_value / 1000;
	value_buf[5] = 0x30 + smog_value % 1000 / 100;
	value_buf[6] = 0x30 + smog_value % 100 / 10;
	value_buf[7] = 0x30 + smog_value % 10;
	//显示烟雾浓度
	LCD1602_Write_Cmd(0x87);
	LCD1602_Write_Dat('S');
	LCD1602_Write_Dat(':');
	LCD1602_Write_Dat(value_buf[4]);
	LCD1602_Write_Dat(value_buf[5]);
	LCD1602_Write_Dat(value_buf[6]);
	LCD1602_Write_Dat(value_buf[7]);
	
	value_buf[8] = 0x30 + beam_value / 1000;
	value_buf[9] = 0x30 + beam_value % 1000 / 100;
	value_buf[10] = 0x30 + beam_value % 100 / 10;
	value_buf[11] = 0x30 + beam_value % 10;
	//显示光照强度
	LCD1602_Write_Cmd(0x80 + 0x40);
	LCD1602_Write_Dat('B');
	LCD1602_Write_Dat(':');
	LCD1602_Write_Dat(value_buf[8]);
	LCD1602_Write_Dat(value_buf[9]);
	LCD1602_Write_Dat(value_buf[10]);
	LCD1602_Write_Dat(value_buf[11]);
	
	value_buf[12] = 0x30 + humi_value / 10;
	value_buf[13] = 0x30 + humi_value % 10;
	LCD1602_Write_Cmd(0x80 + 0x47);
	LCD1602_Write_Dat('H');
	LCD1602_Write_Dat(':');
	LCD1602_Write_Dat(value_buf[12]);
	LCD1602_Write_Dat(value_buf[13]);
	LCD1602_Write_Dat('%');
	
	
	//温度值判断比较
	if(temp_value > 35 && flag1)   
	{
		Relay = 0;//继电器工作、风扇转动
	}
	else if((temp_value < 30 && temp_value >= 15) && flag1) 
	{
		Relay = 1;//继电器关闭，风扇停止转动
	}
	else if(temp_value < 15 && flag1) 
	{
		LED_GREEN = 0;//电亮LED（绿色），模拟室温升高
	}
	
	//烟雾值判断比较
	if(smog_value > 1500)   
	{
		LED_RED = ~LED_RED;//蜂鸣器持续响持续闪烁led灯（红色） 并发送报警消息到app 低于一定值 一定时间后取消报警
		Beep = ~Beep;
		ESP8266_send_data(bbbuf);
		delay_ms(100);
	}
	else
	{
		LED_RED = 1;   //LED，蜂鸣器关闭
		Beep = 1;
	}
	
	if(humi_value)   //湿度超标
	{
	
	}
	else
	{
		
	}
	
	
	//光照值判断
	if((beam_value > 1500 && flag == 1) && flag1)  
	{
		F_Rotation(2000);//开启遮光板（模拟窗帘）。
		LED_YELLOW = 1;
		flag = 2;
	}
	else if((beam_value <= 1500 && flag == 2) && flag1)
	{
		B_Rotation(2000);  //低于一定值关闭遮光板点亮led灯（黄色）
		LED_YELLOW = 0;
		flag = 1;
	}
	
}

//接收来自APP的控制指令
void Receive_data(void)
{
	if(Find("XXXXXX") == 1)
	{
		F_Rotation(2000);
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
		LED_YELLOW = 0;
	}
	
	if(Find("YYYYYY") == 1)
	{
		B_Rotation(2000);
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
		LED_YELLOW = 1;
	}
	
	if(Find("WWWWWW") == 1)
	{
		Relay = 0;//继电器工作、风扇转动
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
	}
	
	if(Find("NNNNNN") == 1)
	{
		Relay = 1;//继电器工作、风扇转动
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
	}
	
	if(Find("KKKKKK") == 1)
	{
		LED_YELLOW = 0;//led亮
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
	}
	
	if(Find("GGGGGG") == 1)
	{
		LED_YELLOW = 1;//LED灭
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
	}
	
	if(Find("ZZZZZZ") == 1)  //开启自动控制
	{
		flag1 = 1;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('A');
	}
}

//系统硬件初始化
void Sys_Init(void)
{
	u8 i;
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(9600);	 //串口初始化为9600，用于调试
	USART3_init(115200);  //串口3初始化，波特率为115200，主要用于WiFi通讯
	LED_Init();		  	 //初始化与LED连接的硬件接口
	Relay_Beep_Init();   //继电器，蜂鸣器初始化
	Motor_Init();       //电机初始化
	DHT11_Init();    //温湿度传感器初始化
	LCD1602_Init();	  //LCD1602初始化
	Adc_Init();     //AD值初始化，用于测量烟雾浓度和光照强度

	while(ESP8266_wifi_Init())    //检测WIFI模块是否插上
	{
		LCD1602_Write_Cmd(0x01);//清屏
		
		//显示连接错误或者模块设置有问题
		LCD1602_Write_Cmd(0x80);
		for(i=0;i<16;i++)
		{
			LCD1602_Write_Dat(bbuf[i]);
		}
	}
	LCD1602_Write_Cmd(0x80);
	for(i=0;i<16;i++)
	{
		LCD1602_Write_Dat(' ');
	}
	//上电手动控制，APP控制
	LCD1602_Write_Cmd(0x80 + 0x4f);
	LCD1602_Write_Dat('M');
	flag = 1;
}


//主函数
 int main(void)
 {	
	Sys_Init();
	delay_ms(1000);
	Data_Deal();
	if(beam_value > 1500)  
	{
		flag = 1;
	}
	else if(beam_value <= 1500)
	{
		flag = 2;
	}
	 
	while(1)
	{
		Data_Deal();  //数据处理
		Receive_data();  //接收来自APP的控制指令
		ESP8266_send_data(value_buf);  //数据上报APP	
	}	 
}


