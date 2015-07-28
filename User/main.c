#include "stm32f10x.h"
#include <stdio.h> 
#include "usart2.h"	  
#include "led.h"	  
#include "key.h"
#include "delay_y.h"	

/*AbleCloud*/
#include "ac_common.h"
#include "ac_hal.h"

	/*设置中断优先级分组*/
void NVIC_Configuration(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	//设置NVIC中断分组2:2位抢占优先级，2位亚优先级
}

int main(void)
{

	NVIC_Configuration();															//设置中断优先级分组
	
	Usart2_init(115200);															//串口2初始化,设置波特率为115200

	LED_Init();		  	//初始化与LED连接的硬件接口
	KEY_Init();
	while(WIFIPOWEROFF==AC_GetStoreStatus(WIFIPOWERSTATUS))
	{
		AC_Init();
		delay_s(1);//延时
		ProcessWifiMsg();
	}
	while(1)
	{
		ProcessWifiMsg();
	}
}
