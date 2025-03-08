#ifndef __WIFI_H
#define __WIFI_H	 
#include "sys.h"

#define Buf3_Max 	  100 //´®¿Ú2»º´æ³¤¶È



void USART3_init(u32 bound);
void CLR_Buf(void);
void UART3_SendString(char* s);
void UART3_Send_Command(char* s);
u8 Find(char *a);
u8 UART3_Send_AT_Command(char *b,char *a,u8 wait_time,u32 interval_time) ;


signed ESP8266_wifi_Init(void);
void ESP8266_send_data(char *buf);
		 				    
#endif
