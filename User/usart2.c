/***********************串口1.2.3发送接收数据*************************/

#include "stm32f10x.h"
#include <stdio.h>
#include "usart2.h"

u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART2_REC_LEN个字节.
u16 USART2_RX_STA=0;       //接收状态标记

u8 USART2RxBuf[USART2RX_RING_LEN];
char PrintCmdPrefix[] = PRINT_FLAG;
char PureDataPrefix[] = RCTRL_MSG_PREFIX;//ARM DATA
//#pragma location = 0x4000
UARTStruct    USART2Port;
u8 pCmdWifiBuf[USART2RX_RING_LEN];

void UARTRx_Buf_Init(UARTStruct *qp, u8 *rxbuf, u16 len)
{
    PKT_DESC     *rx_desc = &(qp->Rx_desc);
    BUFFER_INFO  *rx_ring = &(qp->Rx_Buffer);
    
    rx_desc->pkt_num = 0;
    rx_desc->cur_num = 0;
    rx_desc->cur_type = PKT_UNKNOWN;
    Buf_init(rx_ring,(rxbuf),(u16)len);
}

/******串口2初始化设置,以及数据接收发送设置******/
	/*串口2初始化设置*/
void Usart2_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//使能复用IO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2时钟
	USART_DeInit(USART2);  																	//复位串口2
	
	/*GPIO端口设置*/
	/*USART2_TX   PA.2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 					//PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); 							//初始化PA2
   
	/*USART2_RX	  PA.3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 						//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  							//初始化PA3

	/*Usart2 NVIC 配置*/
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;				//选择串口2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;	//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;				//亚优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);													//根据指定的参数初始化VIC寄存器
  
	/*USART2 初始化设置*/
	USART_InitStructure.USART_BaudRate = bound;																			//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;															//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//使能收发模式

	USART_Init(USART2, &USART_InitStructure); 			//初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启接收中断
	USART_Cmd(USART2, ENABLE);                    	//使能串口2
	
	UARTRx_Buf_Init((UARTStruct*)(&USART2Port),(u8 *)(USART2RxBuf),USART2RX_RING_LEN);
}

	/*串口2发送一个16进制数据*/
void Usart2_Send_Data(u16 Data)
{
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	// 查看发送数据寄存器是否为空
	USART_SendData(USART2,Data);																	// 发送数据
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);  // 等待数据发送完毕,检查USART_FLAG_TC是否置1
}

	/*串口2发送一个字节*/
void Usart2_Send_Ch(u8 ch)
{
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	// 查看发送数据寄存器是否为空
	USART_SendData(USART2,ch);																		// 发送数据
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);   // 等待数据发送完毕,检查USART_FLAG_TC是否置1
}

	/*串口2发送一个字符串*/
void Usart2_Send_Str(u8 *str)
{
	u8 i=0;
	while(str[i]!='\0')
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	// 查看发送数据寄存器是否为空
		USART_SendData(USART2,str[i]);																// 发送数据
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);   // 等待数据发送完毕,检查USART_FLAG_TC是否置1
		i++;
	}
}

	/*串口2发送数组数据*/
void Usart2_Send_Array(u8 send_array[],u8 num) //send_array[]代表要发送的数组的数组数据,//num代表要发送的数据个数，即就是发送该数组的0-num元素
{
	u8 i=0;
	while(i<num)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	// 查看发送数据寄存器是否为空
		USART_SendData(USART2,send_array[i]);													// 发送数据
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);   // 等待数据发送完毕,检查USART_FLAG_TC是否置1
		i++;
	}
}

#if EN_USART2_RX	//如果使能了接收
	/*串口2数据中断接收方式*/
void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	u8 USART2_Rx;


	u16   roomleft = 0;
	PKT_FIFO     *infor;
	PKT_FIFO     *temp_info;
//	u8 ch = 0;
	u8 i= 0;
	
	PKT_DESC     *rx_desc = &(USART2Port.Rx_desc);
	BUFFER_INFO  *rx_ring = &(USART2Port.Rx_Buffer); 
	static u16 AMHeadLen = sizeof(RCTRL_STRU_MSGHEAD);
	static u16 AMBodyLen =0;
	static u8  PDMatchNum = 0;
	static u8  PrintMatchNum = 0;
//	unsigned long ulStatus; 
	Buf_GetRoomLeft(rx_ring,roomleft);

	while(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);					//清除中断标志
		USART2_Rx =USART_ReceiveData(USART2);//(USART1->DR);	//读取接收到的数据
		USART2_RX_BUF[USART2_RX_STA&0X00FF]=USART2_Rx ;
		USART2_RX_STA++;
		switch (rx_desc->cur_type)
		{
			case PKT_UNKNOWN:
			{  
				/**************** detect packet type ***************/
				if (PureDataPrefix[PDMatchNum] == USART2_Rx)
				{         
						PDMatchNum++;
				}
				else
				{         
						PDMatchNum = 0;
				} 
				if (PrintCmdPrefix[PrintMatchNum] == USART2_Rx)
				{         
						PrintMatchNum++;
				}
				else
				{         
						PrintMatchNum = 0;
				}         
				if ((PDMatchNum == sizeof(PureDataPrefix)-1) ||  
						(PrintMatchNum == sizeof(PrintCmdPrefix)-1))   //match case 3:arm  data
				{   
						
						rx_desc->cur_num = rx_desc->pkt_num;                  
						infor = &(rx_desc->infor[rx_desc->cur_num]);
						infor->pkt_len = 0;
						
						if (PrintMatchNum == sizeof(PrintCmdPrefix)-1)
						{            
								rx_desc->cur_type = PKT_PRINTCMD;           //match case 2:iwpriv ra0
						}
						else if (PDMatchNum == sizeof(PureDataPrefix)-1)
						{            
								rx_desc->cur_type = PKT_PUREDATA;           //match case 2:iwpriv ra0
								if(roomleft<AMHeadLen)
								{
										rx_desc->cur_type= PKT_UNKNOWN;
								}
								else
								{
										for(i = 0;i < sizeof(PureDataPrefix)-1;i++)
										{
												Buf_Push(rx_ring,PureDataPrefix[i]);
										}
										roomleft= roomleft-sizeof(PureDataPrefix)+1;
										infor = &(rx_desc->infor[rx_desc->cur_num]);
										infor->pkt_len = infor->pkt_len + i;
								}                                       
						}
						PrintMatchNum = 0;
						PDMatchNum = 0;
						continue;
					}           
			}
			break;
			case PKT_PRINTCMD:
			{          
					/*
					* received one complete packet
					*/
					if(USART2_Rx == '\0'||USART2_Rx == '\n' || USART2_Rx == '\r')
					{   
							rx_desc->cur_type = PKT_UNKNOWN;
							return;
					}
			}
			break;
			
			case PKT_PUREDATA:
			{   
				infor = &(rx_desc->infor[rx_desc->cur_num]);
				Buf_Push(rx_ring,USART2_Rx);
				roomleft--;
				infor->pkt_len++;
				if(infor->pkt_len==AC_PAYLOADLENOFFSET)
				{
						AMBodyLen = USART2_Rx;
				}
				else if(infor->pkt_len==(AC_PAYLOADLENOFFSET +1))
				{
						AMBodyLen = (AMBodyLen<<8) + USART2_Rx;
				}   
				/*
				* if overflow,we discard the current packet
				* example1:packet length > ring size
				* example2:rx ring buff can no be freed by task as quickly as rx interrupt coming
				*/    
				if ((!roomleft) || (rx_desc->pkt_num >= NUM_DESCS))
				{   
						//rollback
						Buff_RollBack(rx_ring,infor->pkt_len);
						
						roomleft += infor->pkt_len;
						
						infor->pkt_type = PKT_UNKNOWN;
						infor->pkt_len = 0;
						rx_desc->cur_type = PKT_UNKNOWN;
						
						if (rx_desc->pkt_num >= NUM_DESCS)
						{
								rx_desc->pkt_num--;
						}
						
				}      
				/*
				* received one complete packet
				*/
				if(AMHeadLen+AMBodyLen==infor->pkt_len)
				{   
						//if task has consumed some packets
						if (rx_desc->cur_num != rx_desc->pkt_num)
						{   
								temp_info = infor;
								infor     = &(rx_desc->infor[rx_desc->pkt_num]);
								infor->pkt_len = temp_info->pkt_len;
								temp_info->pkt_len = 0;
								temp_info->pkt_type = PKT_UNKNOWN;
						}
						
						infor->pkt_type = rx_desc->cur_type;  // PKT_ATCMD / PKT_IWCMD;
						rx_desc->pkt_num++;
						rx_desc->cur_type = PKT_UNKNOWN;
						AMBodyLen =0;
						return;                    
				}
			}
			break;
			default:
			break;
		}  
	}
}

void ProcessWifiMsg(void)
{
    PKT_FIFO     *infor;
    PKT_DESC     *rx_desc = &(USART2Port.Rx_desc);
    BUFFER_INFO  *rx_ring = &(USART2Port.Rx_Buffer); 
    
    PKT_TYPE rxpkt_type;
    u16   rxpkt_len;
    u16 i;
    
    while (rx_desc->pkt_num)
    {   
        //simulate FIFO,1st in,1st out
        infor = &(rx_desc->infor[0]);
        rxpkt_type = infor->pkt_type;
        rxpkt_len  = infor->pkt_len;
        
        memset(pCmdWifiBuf, 0, USART2RX_RING_LEN);
        
        //copy from uart rx ring
        for(i = 0;i < rxpkt_len;i++)       //O(n)
        {
            Buf_Pop(rx_ring,pCmdWifiBuf[i]);
            //Printf_High("Buf_Pop=%x \n",pCmdBuf[i]);
        }
        
        //reset value
        infor->pkt_type = PKT_UNKNOWN;
        infor->pkt_len = 0;
        
        //shift FIFO
        for (i=0; i<(rx_desc->pkt_num)-1; i++)  //O(n)
        {
            rx_desc->infor[i]= rx_desc->infor[i+1];
        }  
        rx_desc->pkt_num--;
        
        //handle previous packet
        switch (rxpkt_type)
        {
            case PKT_PUREDATA:
                AC_RecvMessage((AC_MessageHead*)(pCmdWifiBuf + 4));
                break;
            default:
            break;
        }    
    }   
}
#endif	
