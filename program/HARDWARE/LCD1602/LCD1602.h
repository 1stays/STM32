#ifndef _lcd1602_H
#define _lcd1602_H


#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "delay.h"

#define	LCD1602_RS	PCout(13)
#define	LCD1602_RW	PCout(14)
#define	LCD1602_EN  PCout(15)
#define	GPIO_WriteLow(GPIOA,x)  (GPIOA->BSRR = ((~(x)<<16)|(x)))


void LCD1602_Init(void);
void LCD1602_Wait_Ready(void);
void LCD1602_Write_Cmd(u8 cmd);
void LCD1602_Write_Dat(u8 dat);
void LCD1602_ClearScreen(void);
void LCD1602_Set_Cursor(u8 x, u8 y);
void LCD1602_Show_Str(u8 x, u8 y, u8 *str);
void LCD1602_GPIOInit(void);





#endif

