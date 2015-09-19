/**
******************************************************************************
* @file     ac_app_main.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief   
******************************************************************************
*/

#include <ac_common.h>
#include <stm32f10x.h>
#include <ac_protocol_interface.h>
#include <ac_api.h>
#include <ac_hal.h>
#include <ac_cfg.h>
#include <ac_klv.h>
#include "cJSON.h"
#include "usart2.h"	  
#include "led.h"	  
#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
u8  g_u8EqVersion[AC_EQVERSION_LEN]={0,0,0,0};      
u8  g_u8ModuleKey[AC_MODULE_KEY_LEN] = DEFAULT_IOT_PRIVATE_KEY;
u64  g_u64Domain = ((((u64)MAJOR_DOMAIN_ID)<<16) + (u64)SUB_DOMAIN_ID) ;
u8  g_u8DeviceId[AC_HS_DEVICE_ID_LEN];
u32 g_u32CloudStatus = CLOUDDISCONNECT;
typedef struct tag_STRU_LED_ONOFF
{		
    u8	     u8LedOnOff ; // 0:�أ�1������2����ȡ��ǰ����״̬
    u8	     u8ControlStatus;//0ΪAPP���ƿ��أ�1Ϊ�������ƿ���	
    u8	     u8Pad[2];		 
}STRU_LED_ONOFF;

u32 g_u32WifiPowerStatus = WIFIPOWEROFF;

extern u8 g_u8MsgBuildBuffer[AC_MESSAGE_MAX_LEN];

/*************************************************
* Function: AC_BlinkLed
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
ErrorStatus  AC_BlinkLed(unsigned char blink)
{
    if (blink)
	{
	    GPIO_ResetBits(GPIOA,GPIO_Pin_8); //LED0�����
	}
	else
    {
		GPIO_SetBits(GPIOA,GPIO_Pin_8);//LED0�����
    }
    return SUCCESS;
}

/*************************************************
* Function: AC_SendDevStatus2Server
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_SendLedBinaryStatus2Server(u8 u8control)
{
    STRU_LED_ONOFF struReport;
    u16 u16DataLen;
    struReport.u8LedOnOff = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8);
    AC_BuildMessage(AC_CODE_BINARY_REPORT,0,
                    (u8*)&struReport, sizeof(STRU_LED_ONOFF),
                    NULL, 
                    g_u8MsgBuildBuffer, &u16DataLen);

    AC_SendMessage(g_u8MsgBuildBuffer, u16DataLen);
}
/*************************************************
* Function: AC_SendLedKlvStatus2Server
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_SendLedKlvStatus2Server(u8 u8value)
{
     /*�ϱ�demo�Ƶ�״̬*/
    u8 u8LedOnOff ;
    /*KLVЭ���ڴ����*/
    AC_KLV *pOut = AC_CreateObj();
      /*��ȡdemo��״̬*/
    u8LedOnOff = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8);
    /*����KLV��Ϣ*/
    AC_SetKeyValue(pOut,KEY_LED_ON_OFF,sizeof(u8LedOnOff),INT8_TYPE,&u8LedOnOff);
    AC_SetKeyValue(pOut,KEY_LED_CONTROL_STATUS,sizeof(u8value),INT8_TYPE,&u8value);
      /*�ϱ�KLV��Ϣ*/
    AC_ReportKLVMessage(AC_CODE_KLV_REPORT, NULL, pOut);
    /*KLVЭ���ڴ��ͷ�*/
    AC_FreeObj(pOut);
}

/*************************************************
* Function: AC_SendDevStatus2Server
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_SendLedJsonStatus2Server(u8 controltype)
{
    cJSON *root;
    char *out;
    u8 u8LedOnOff;
    u16 u16DataLen;
    root=cJSON_CreateObject();
    u8LedOnOff =  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8);
    cJSON_AddNumberToObject(root,"action",		u8LedOnOff);
    cJSON_AddNumberToObject(root,"controltype",		controltype);

    out=cJSON_Print(root);	
    cJSON_Delete(root);
    AC_BuildMessage(AC_CODE_JSON_REPORT,0,
                    (u8*)out, strlen(out),
                    NULL, 
                    g_u8MsgBuildBuffer, &u16DataLen);
    AC_SendMessage(g_u8MsgBuildBuffer, u16DataLen);	
    free(out);
}
/*************************************************
* Function: AC_Init
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_Init()
{
    u8 u8DevceIdLen = sizeof(DEVICE_ID) -1;
    AC_SendDeviceStart(NULL);
    memset(g_u8DeviceId,0x30,sizeof(g_u8DeviceId));
    memcpy(g_u8DeviceId+(AC_HS_DEVICE_ID_LEN -u8DevceIdLen),DEVICE_ID,u8DevceIdLen);
}

/*************************************************
* Function: AC_ConfigWifi
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_ConfigWifi(void)
{
    AC_Configuration struConfig;
    u8 u8CloudKey[AC_CLOUD_KEY_MAX_LEN] = DEFAULT_IOT_CLOUD_KEY;
    struConfig.u32TraceSwitch = AC_HTONL(0);     //Trace data switch, 1:open, 0:close,default 0
    struConfig.u32SecSwitch = AC_HTONL(0);       //Sec data switch, 1:open, 0:close, 2:close RSA, default 1
    struConfig.u32WifiConfig =  AC_HTONL(0);      //Use Config SSID,password,1:open, 0:close, default 0
    struConfig.u32TestAddrConfig = AC_HTONL(0);      //connect with test url,1:ip, 0:dns, default 0
    memcpy(struConfig.u8Ssid, "HW_test", 7);
    memcpy(struConfig.u8Password, "87654321", 8);
    struConfig.u32IpAddr = AC_HTONL(0xc0a8c772);  //local test ip
    struConfig.u16Port = AC_HTONS(9100); 

    memcpy(struConfig.u8CloudAddr, "test.ablecloud.cn", AC_CLOUD_ADDR_MAX_LEN);
    memcpy(struConfig.u8CloudKey, u8CloudKey, AC_CLOUD_KEY_MAX_LEN);

    AC_SendDeviceConfig(NULL, &struConfig);
}
/*************************************************
* Function: AC_SendMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_SendMessage(u8 *pu8Msg, u16 u16DataLen)
{
    u8 u8MagicHead[4] = {0x02,0x03,0x04,0x05};
    Usart2_Send_Array ( u8MagicHead,  sizeof(u8MagicHead));
    Usart2_Send_Array ( pu8Msg,  u16DataLen);
}

/*************************************************
* Function: AC_DealNotifyMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealNotifyMessage(AC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{
    
    //����wifiģ���֪ͨ����Ϣ
    switch(pstruMsg->MsgCode)
    {
        case AC_CODE_EQ_DONE://wifiģ������֪ͨ
        AC_StoreStatus(WIFIPOWERSTATUS , WIFIPOWERON);
        AC_ConfigWifi();
        AC_Printf("Wifi Power On!\n");
        break;
        case AC_CODE_WIFI_CONNECTED://wifi���ӳɹ�֪ͨ
        //AC_SendDeviceRegsiterWithMac(g_u8EqVersion,g_u8ModuleKey,g_u64Domain);
        AC_SendDeviceRegsiter(g_u8EqVersion,g_u8ModuleKey,g_u64Domain,g_u8DeviceId);
        AC_Printf("Wifi Connect!\n");
        break;
        case AC_CODE_WIFI_DISCONNECTED://�ƶ�����֪ͨ
        AC_Printf("Wifi DisConnect!\n");
        break;
        case AC_CODE_CLOUD_CONNECTED://�ƶ�����֪ͨ
        AC_StoreStatus(CLOUDSTATUS,CLOUDCONNECT);
        AC_Printf("Cloud Connect!\n");
        break;
        case AC_CODE_CLOUD_DISCONNECTED://�ƶ˶���֪ͨ
        AC_StoreStatus(CLOUDSTATUS,CLOUDDISCONNECT);
        AC_Printf("Cloud DisConnect!\n");
        break;
    }
}

/*************************************************
* Function: AC_DealOtaMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealOtaMessage(AC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{
    //����wifiģ���OTA����Ϣ����Demoû��ʵ�ָù���
    switch(pstruMsg->MsgCode)
    {
        case AC_CODE_OTA_BEGIN:
        {
            AC_Printf("ota begin\n");
        }
        break; 
        case AC_CODE_OTA_FILE_BEGIN:
        {
            AC_Printf("ota file begin\n");
        }
        break;
        case AC_CODE_OTA_FILE_CHUNK:
        {
            AC_Printf("ota AC_CODE_OTA_FILE_CHUNK begin\n");
        }
        break;
        case AC_CODE_OTA_FILE_END:
        {
            AC_Printf("ota AC_CODE_OTA_FILE_END begin\n");
        }
        break;
        case AC_CODE_OTA_END:
        {
            AC_Printf("ota AC_CODE_OTA_END begin\n");
        }
        break;
        case AC_CODE_OTA_CONFIRM:
        {
            AC_Printf("ota AC_CODE_OTA_CONFIRM begin\n");
            AC_SendRebootMsg(pstruOptList);
        }
        break;
    }    
    AC_SendAckMsg(pstruOptList, pstruMsg->MsgId);
}
/*************************************************
* Function: AC_TestKlvMessag
* Description: �ػ�����
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_TestKlvMessag(AC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{   
    //�ػ�����

    AC_KLV *pOut = AC_CreateObj();

    u8 u8out= 0;
    u16 u16out = 0;
    u32 u32out = 0;
    u64 u64out = 0;
    float f32out = 0;
    double f64out = 0;
    u8 stringout[20] = {0};
    u16 u16length = 0;
    u8 u8Type = 0;
    
    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),1,NULL,&u16length,&u8Type);
    AC_SetKeyValue(pOut,1,u16length,u8Type,NULL);
    
    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),2,&u8out,&u16length,&u8Type);
    AC_SetKeyValue(pOut,2,u16length,u8Type,&u8out);

    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),3,&u8out,&u16length,&u8Type);
    AC_SetKeyValue(pOut,3,u16length,u8Type,&u8out);
    
    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),4,&u16out,&u16length,&u8Type);
    AC_SetKeyValue(pOut,4,u16length,u8Type,&u16out);
    
    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),5,&u32out,&u16length,&u8Type);
    AC_SetKeyValue(pOut,5,u16length,u8Type,&u32out);
      
    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),6,&u64out,&u16length,&u8Type);
    AC_SetKeyValue(pOut,6,u16length,u8Type,&u64out);
    
    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),7,&f32out,&u16length,&u8Type);
    AC_SetKeyValue(pOut,7,u16length,u8Type,&f32out);
    
    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),0,&f64out,&u16length,&u8Type);
    AC_SetKeyValue(pOut,0,u16length,u8Type,&f64out); 
    
    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),255,stringout,&u16length,&u8Type);
    AC_SetKeyValue(pOut,255,u16length,u8Type,stringout);

    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),12,stringout,&u16length,&u8Type);
    AC_SetKeyValue(pOut,12,u16length,u8Type,stringout);
    
    AC_SendKLVMessage(pstruMsg, pstruOptList,pOut);
    AC_ReportKLVMessage(201, pstruOptList, pOut);
    AC_FreeObj(pOut);
}

/*************************************************
* Function: AC_DealKLVMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/

void AC_DealBinaryMessage(AC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{
    u16 u16DataLen;
    u8 resp[4] = {0};
    
    switch (((STRU_LED_ONOFF *)pu8Playload)->u8LedOnOff)
    {
        case 0://��������Ϣ
        case 1:        
        resp[0]=AC_BlinkLed(((STRU_LED_ONOFF *)pu8Playload)->u8LedOnOff);
        break;            
        
    }
    /*������Ϣ,�ӿں�������½ڽӿڶ���*/
    AC_BuildMessage(CLIENT_SERVER_OK,pstruMsg->MsgId,
                    (u8*)resp, sizeof(resp),
                    pstruOptList, 
                    g_u8MsgBuildBuffer, &u16DataLen);
    /*������Ϣ,�ӿں�������½ڽӿڶ���*/
    AC_SendMessage(g_u8MsgBuildBuffer, u16DataLen);    
    
    AC_SendLedBinaryStatus2Server(KEY_LED_CONTROL_FROMAPP);
}
/*************************************************
* Function: AC_DealKLVMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealKLVMessage(AC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{   
    //����KLV������Ϣ
    AC_KLV *pOut = AC_CreateObj();
    u8 u8LedOnOff= 0;
    u16 u16length = 0;
    u8 u8Type = 0;
    ErrorStatus ret = (ErrorStatus)ERROR;
    AC_GetKeyValue(pu8Playload,AC_HtoNs(pstruMsg->Payloadlen),KEY_LED_ON_OFF,&u8LedOnOff,&u16length,&u8Type);
    switch (u8LedOnOff)
    {
        case 0://��������Ϣ
        case 1:        
        ret = AC_BlinkLed(u8LedOnOff);
        break;            
        
    }
    /*����KLV��Ϣ*/
    AC_SetKeyValue(pOut,KEY_LED_ON_OFF,sizeof(u8LedOnOff),BOOL_TYPE,&ret);
    /*������Ӧ��Ϣ���û��Զ���*/
    AC_SendKLVMessage(pstruMsg, pstruOptList,pOut);
    /*KLVЭ���ڴ��ͷ�*/
    AC_FreeObj(pOut); 
    AC_SendLedKlvStatus2Server(KEY_LED_CONTROL_FROMAPP);
}


/*************************************************
* Function: AC_DealEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealJsonMessage(AC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{   
    //�����豸�Զ��������Ϣ
    u16 u16DataLen;
    u32 u32LedOnOff;
    ErrorStatus result = (ErrorStatus)ERROR;
    char *out;
    cJSON *root;
    cJSON *format = cJSON_Parse((const char *)pu8Playload);
    if(format)
    {
        u32LedOnOff = cJSON_GetObjectItem(format,"switch")->valueint;
        switch (u32LedOnOff)
        {
            case 0://��������Ϣ
            case 1:        
            result = AC_BlinkLed(u32LedOnOff);
            break;
        }
        cJSON_Delete(format);
    }
    
    root=cJSON_CreateObject();
    cJSON_AddBoolToObject(root,"result",result);
    out=cJSON_Print(root);	
    cJSON_Delete(root);
    AC_BuildMessage(AC_CODE_JSON_RSP,pstruMsg->MsgId,
                    (u8*)out, strlen(out),
                    NULL, 
                    g_u8MsgBuildBuffer, &u16DataLen);
    AC_SendMessage(g_u8MsgBuildBuffer, u16DataLen);	
    free(out);
    
    AC_SendLedJsonStatus2Server(KEY_LED_CONTROL_FROMAPP);
}
/*************************************************
* Function: AC_DealEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealEvent(AC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{   
    //�����豸�Զ��������Ϣ
    switch(pstruMsg->MsgCode)
    {
        case AC_CODE_BINARY_LED:
        AC_DealBinaryMessage(pstruMsg, pstruOptList, pu8Playload);
        break; 
        case AC_CODE_KLV_LED:
        AC_DealKLVMessage(pstruMsg, pstruOptList, pu8Playload);
        break;
        case AC_CODE_JSON_LED:
        AC_DealJsonMessage(pstruMsg, pstruOptList, pu8Playload);
        break; 
    }
}

/*************************************************
* Function: AC_DealEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_StoreStatus(u32 u32Type , u32 u32Data)
{
    
    switch(u32Type)
    {
        case CLOUDSTATUS:
        g_u32CloudStatus = u32Data;
        break;
        case WIFIPOWERSTATUS:
        g_u32WifiPowerStatus = u32Data;
        break;
    }
}

/*************************************************
* Function: AC_DealEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 AC_GetStoreStatus(u32 u32Type)
{
    switch(u32Type)
    {
        case CLOUDSTATUS:
        return g_u32CloudStatus;
        case WIFIPOWERSTATUS:
        return g_u32WifiPowerStatus;
    }
   return AC_RET_ERROR;
}

