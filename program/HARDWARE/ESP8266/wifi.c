#include "wifi.h"
#include "string.h"
#include "delay.h"

///////////////串口基本设置//////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

/*************  本地变量声明	**************/
char Uart3_Buf[Buf3_Max];//串口2接收缓存
u8 First_Int = 0;

//串口3初始化
void USART3_init(u32 bound)
{
  //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART3，GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3，GPIOB时钟
	
	//USART3_TX   GPIOB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB

	//USART3_RX	  GPIOB11初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB  

	//Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART3, ENABLE);                    //使能串口3

}

void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	u8 Res=0;
	Res = USART_ReceiveData(USART3);
	Uart3_Buf[First_Int] = Res;  	  //将接收到的字符串存到缓存中
	First_Int++;                	  //缓存指针向后移动
	if(First_Int >= Buf3_Max)       	  //如果缓存满,将缓存指针指向缓存的首地址
	{
		First_Int = 0;
	}
} 

//清除串口缓存数据
void CLR_Buf(void)
{
	u16 k;
	for(k=0;k<Buf3_Max;k++)      //将缓存内容清零
	{
		Uart3_Buf[k] = 0x00;
	}
    First_Int = 0;              //接收字符串的起始存储位置
}

//发送字符串
void UART3_SendString(char* s)
{
	while(*s)//检测字符串结束符
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET); 
		USART_SendData(USART3 ,*s++);//发送当前字符
	}
}

//发送命令
void UART3_Send_Command(char* s)
{
	CLR_Buf(); //清空接收数据的buffer
	UART3_SendString(s); //发出字符串
	UART3_SendString("\r"); //再自动发送 \r\n两个字符
}


//寻找字符串
//返回：1  已找到  0  未找到
u8 Find(char *a)
{ 
	if(strstr(Uart3_Buf, a)!=NULL)
	{
		return 1;
	}	
	else
	{
		return 0;
	}
		
}


//发送AT指令
//*b：需要发送的字符串
//*a：查找是否返回的字符串
//wait_time：发送的次数
//interval_time:每次等待的时间
u8 UART3_Send_AT_Command(char *b,char *a,u8 wait_time,u32 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)    //如果没有找到 就继续再发一次指令 再进行查找目标字符串                
	{
		UART3_Send_Command(b);//串口2发送 b 字符串 他会自动发送\r\n  相当于发送了一个指令
		delay_ms(interval_time); //等待一定时间 传50的话就是 50*20ms = 1秒
		if(Find(a))            //查找需要应答的字符串 a
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

//设置WIFI模块模式
//多连接 AP 接入点名称 密码  通道号  WPA2_PSK 
signed ESP8266_wifi_Init(void)
{
	u8 ret;
	ret = UART3_Send_AT_Command("AT\r\n","OK",3,100);    //检测模块是否存在
	if(ret == 0)
	{
		return -1;
	}
	
	ret = UART3_Send_AT_Command("AT+CWMODE=2\r\n","OK",3,100);   //设置模块为AP模式
	if(ret == 0)
	{
		return -2;
	}
	
	ret = UART3_Send_AT_Command("AT+CWSAP=\"wifi\",\"12345678\",5,3\r\n","OK",3,100);  //设置接入点名称 密码  通道号  WPA2_PSK
	if(ret == 0)
	{
		return -3;
	}
	
	ret = UART3_Send_AT_Command("AT+CIPMUX=1\r\n","OK",3,100); //设置为多连接
	if(ret == 0)
	{
		return -4;
	}
	
	ret = UART3_Send_AT_Command("AT+CIPSERVER=1,5000\r\n","OK",3,100);  //设置模块为TCP服务器，端口号为5000
	if(ret == 0)
	{
		return -5;
	}
	
	ret = UART3_Send_AT_Command("AT+CIPSTO=0\r\n","OK",3,100);   //设置超时时间为0
	if(ret == 0)
	{
		return -6;
	}
	
	return 0;
}

//发送数据
//*buf：需要发送的字符串
void ESP8266_send_data(char *buf)
{
	UART3_Send_AT_Command("AT+CIPSEND=0,14\r\n",">",3,50);   //发送通道号，以及需要发送的字符个数
	delay_ms(200);
	UART3_SendString(buf);
	delay_ms(100);
}
















