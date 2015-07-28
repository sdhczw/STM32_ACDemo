/**********************DELAY*************************/

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "delay_y.h"	


void delay_s(u16 s)//—” ±√Î
{
	u16 i,j,k;
	for(i=0;i<s;i++)
	{
		for(j=0;j<8000;j++)
		{
			for(k=0;k<1000;k++)
			{
			}
		}
	}
}

void delay_ms(u16 ms)//—” ±∫¡√Î
{
	u16 i,j;
	for(i=0;i<ms;i++)
	{
		for(j=0;j<8000;j++)
		{
//			for(k=0;k<1000;k++)
//			{
//			}
		}
	}
}
