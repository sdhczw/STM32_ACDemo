/***********************串口1.2.3发送接收数据*************************/

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "led.h"	  
#include "delay_y.h"	

void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PB.5 输出高
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PE.5 输出高 
}

void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_8); //LED1输出低	
}	
void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_8);//LED1输出高
}	
void LED2_ON(void)
{
GPIO_ResetBits(GPIOD,GPIO_Pin_2);//LED2输出低
}	
void LED2_OFF(void)
{
	GPIO_SetBits(GPIOD,GPIO_Pin_2);//LED2输出高
}	

