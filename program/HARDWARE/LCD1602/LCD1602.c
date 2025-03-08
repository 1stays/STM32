#include "lcd1602.h"

void LCD1602_GPIOInit(void)
{
	//初始化IO口
    GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);	 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

void LCD1602_Write_Cmd(u8 cmd)
{
	LCD1602_RS=0;
	LCD1602_RW=0;
    LCD1602_EN=0;
	GPIO_WriteLow(GPIOA,cmd);
    delay_us(5);
	LCD1602_EN=1;
	delay_ms(2);
	LCD1602_EN=0;
    
}



void LCD1602_Write_Dat(u8 dat)
{
	LCD1602_RS=1;
	LCD1602_RW=0;
    LCD1602_EN=0;
	GPIO_WriteLow(GPIOA,dat);
    delay_us(5);
	LCD1602_EN=1;
	delay_ms(2);
	LCD1602_EN=0;
    
}



void LCD1602_ClearScreen(void)
{
	LCD1602_Write_Cmd(0x01);
}


void LCD1602_Set_Cursor(u8 x, u8 y)
{
	if(y==0)
		LCD1602_Write_Cmd(0x80+x);
	else
		LCD1602_Write_Cmd(0x80+0x40+x);;
}


void LCD1602_Show_Str(u8 x,u8 y,u8 *str)
{
    LCD1602_Set_Cursor(x, y);
	while(*str!='\0')
	{
		LCD1602_Write_Dat(*str++);
	}
   
}

void LCD1602_Init()
{
	LCD1602_GPIOInit();
    //初始化LCD1602
    LCD1602_Write_Cmd(0x38);//设置8位格式,2行,5x7
    LCD1602_Write_Cmd(0x0c);//整体显示,关光标,不闪烁
    LCD1602_Write_Cmd(0x06);//设定输入方式,文字不动,地址自动+1
    LCD1602_Write_Cmd(0x01);//清屏
	delay_ms(5); 
}



