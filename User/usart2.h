#ifndef USART2_H
#define USART2_H
#include "stdio.h"	

#include "bmd.h"
#include "ac_api.h"
#include <ac_common.h>
#include <ac_protocol_interface.h>

#define USART2_REC_LEN  			100  	//�����������ֽ��� 100
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������2����

/*AC*/
#define RCTRL_MSG_PREFIX		"\2\3\4\5"
#define PRINT_FLAG                      "\1\2\3\4"




//how many packets can be buffered in rxring ,(each packet size need < UARTRX_RING_LEN - 1)
#define    NUM_DESCS           30

//buffer length for uart rx buffer whose data is moved from uart UART HW RX FIFO
#define    USART2RX_RING_LEN    1024   
#define    AC_PAYLOADLENOFFSET  9





extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART2_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART2_RX_STA;         		//����״̬���




typedef enum {
    PKT_UNKNOWN,
    PKT_PUREDATA,
    PKT_PRINTCMD,
} PKT_TYPE;

typedef struct tag_RCTRL_STRU_MSGHEAD
{
    u32 MsgFlag;         //��Ϣ��ʾ byte0:0X76, byte1:0x81, byte2:0x43, byte3:0x50 0x76814350
    AC_MessageHead  struMessageHead;
}RCTRL_STRU_MSGHEAD;    //��Ϣͷ����




typedef struct {
    PKT_TYPE pkt_type;
    u16   pkt_len;
}PKT_FIFO;//packet infor is in sequence with index[0,num_desc-1] which mapping the sequence in rx


/*
* 2014/05/13,terrence
* struct to descrypt the packet in ring buffer
* we just need to know the packet number and their lengh in sequence
*/
typedef struct {
    PKT_TYPE  cur_type;              //receiving packet:which packet type is receiving current? 
    u8     cur_num;               //receiving packet:current index of receiving packet
    u8     pkt_num;               //completely packets:packet number in ring buffer
    PKT_FIFO  infor[NUM_DESCS];      //completely packets:FIFO,packet infor in ring
} PKT_DESC; 


typedef struct
{
    BUFFER_INFO                    Rx_Buffer;  //receive buffer
    PKT_DESC                       Rx_desc;    //description       
    
    BUFFER_INFO                    Tx_Buffer;  //transmit buffer
    
} UARTStruct;

// ================================================================
//				Golbal	Variable Definition
// ================================================================

extern u8 g_u8_wifi_powstatus;

extern u8 g_u8_machineType;
//*****************************************************************************
//
//! fuction
//
//*****************************************************************************

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


extern void ProcessWifiMsg(void);

//����봮���жϽ��գ��벻Ҫע�����º궨��
extern void Usart2_init(u32 bound);											//��������2��ʼ������
extern void Usart2_Send_Data(u16 Data);									//��������2����16�������ݺ���
extern void Usart2_Send_Ch(u8 ch);											//��������2�����ֽں���
extern void Usart2_Send_Str(u8 *str);										//��������2�����ַ�������
extern void Usart2_Send_Array(u8 send_array[],u8 num);	//��������2�������麯��
#endif
