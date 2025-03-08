#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED_RED PBout(5)	// PB5
#define LED_GREEN PBout(6)	// PB6
#define LED_YELLOW PBout(7)	// PB7

#define motor1 PBout(8)	// PA10
#define motor2 PBout(9)	// PA11
#define motor3 PAout(12)	// PA12
#define motor4 PAout(15)	// PA15

#define Relay PBout(13)	// PB13
#define Beep PBout(12)	// PB12

void LED_Init(void);//≥ı ºªØ
void Relay_Beep_Init(void);
void Motor_Init(void);

		 				    
#endif
