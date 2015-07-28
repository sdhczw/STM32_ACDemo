/***********************����1.2.3���ͽ�������*************************/

#include "stm32f10x.h"
#include <stdio.h>
#include "usart2.h"

u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART2_REC_LEN���ֽ�.
u16 USART2_RX_STA=0;       //����״̬���

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

/******����2��ʼ������,�Լ����ݽ��շ�������******/
	/*����2��ʼ������*/
void Usart2_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//ʹ�ܸ���IOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART2ʱ��
	USART_DeInit(USART2);  																	//��λ����2
	
	/*GPIO�˿�����*/
	/*USART2_TX   PA.2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 					//PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); 							//��ʼ��PA2
   
	/*USART2_RX	  PA.3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 						//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  							//��ʼ��PA3

	/*Usart2 NVIC ����*/
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;				//ѡ�񴮿�2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;	//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;				//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);													//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	/*USART2 ��ʼ������*/
	USART_InitStructure.USART_BaudRate = bound;																			//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;															//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//ʹ���շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); 			//��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//���������ж�
	USART_Cmd(USART2, ENABLE);                    	//ʹ�ܴ���2
	
	UARTRx_Buf_Init((UARTStruct*)(&USART2Port),(u8 *)(USART2RxBuf),USART2RX_RING_LEN);
}

	/*����2����һ��16��������*/
void Usart2_Send_Data(u16 Data)
{
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	// �鿴�������ݼĴ����Ƿ�Ϊ��
	USART_SendData(USART2,Data);																	// ��������
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);  // �ȴ����ݷ������,���USART_FLAG_TC�Ƿ���1
}

	/*����2����һ���ֽ�*/
void Usart2_Send_Ch(u8 ch)
{
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	// �鿴�������ݼĴ����Ƿ�Ϊ��
	USART_SendData(USART2,ch);																		// ��������
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);   // �ȴ����ݷ������,���USART_FLAG_TC�Ƿ���1
}

	/*����2����һ���ַ���*/
void Usart2_Send_Str(u8 *str)
{
	u8 i=0;
	while(str[i]!='\0')
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	// �鿴�������ݼĴ����Ƿ�Ϊ��
		USART_SendData(USART2,str[i]);																// ��������
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);   // �ȴ����ݷ������,���USART_FLAG_TC�Ƿ���1
		i++;
	}
}

	/*����2������������*/
void Usart2_Send_Array(u8 send_array[],u8 num) //send_array[]����Ҫ���͵��������������,//num����Ҫ���͵����ݸ����������Ƿ��͸������0-numԪ��
{
	u8 i=0;
	while(i<num)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	// �鿴�������ݼĴ����Ƿ�Ϊ��
		USART_SendData(USART2,send_array[i]);													// ��������
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);   // �ȴ����ݷ������,���USART_FLAG_TC�Ƿ���1
		i++;
	}
}

#if EN_USART2_RX	//���ʹ���˽���
	/*����2�����жϽ��շ�ʽ*/
void USART2_IRQHandler(void)                	//����1�жϷ������
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

	while(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);					//����жϱ�־
		USART2_Rx =USART_ReceiveData(USART2);//(USART1->DR);	//��ȡ���յ�������
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
