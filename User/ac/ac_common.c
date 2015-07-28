//*****************************************************************************
//
// common.h - BigLittleSwap 
//
// Copyright (c) 2012-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************
#include "ac_common.h"

// ������˷���1��С�˷���0
#define BigLittleSwap16(A)  ((((u16)(A) & 0xff00) >> 8) | \
                                                 (((u16)(A) & 0x00ff) << 8))

// �����ʹ�С�˻���
#define BigLittleSwap32(A)  ((((u32)(A) & 0xff000000) >> 24) | \
                                                 (((u32)(A) & 0x00ff0000) >> 8) | \
                                                 (((u32)(A) & 0x0000ff00) << 8) | \
                                                 (((u32)(A) & 0x000000ff) << 24))
#define BigLittleSwap64(A)    (((u64)(A) >> 56)| \
                                   (((u64)(A) & 0x00ff000000000000) >> 40)| \
                                   (((u64)(A) &0x0000ff0000000000) >> 24)| \
                                   (((u64)(A) & 0x000000ff00000000) >> 8)   | \
                                   (((u64)(A) & 0x00000000ff000000) << 8)   | \
                                   (((u64)(A) & 0x0000000000ff0000) << 24) | \
                                   (((u64)(A) & 0x000000000000ff00) << 40)  | \
                                   (((u64)(A)  << 56)))  
/*************************************************
* Function: checkCPUendian
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
s32 AC_checkCPUendian()
{

       union{

              u32 i;

              unsigned char s[4];

       }c;

 

       c.i = 0x12345678;

       return (0x12 == c.s[0]);

}

 
/*************************************************
* Function: HtoNl
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/

u32 AC_HtoNl(u32 h)
{

       // ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���

       // ������ΪС�ˣ�ת���ɴ���ٷ���

       return AC_checkCPUendian() ? h : BigLittleSwap32(h);

}

 
/*************************************************
* Function: NtoHl
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/

u32 AC_NtoHl(u32 n)
{

       // ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���

       // ������ΪС�ˣ���������ת����С���ٷ���

       return AC_checkCPUendian() ? n : BigLittleSwap32(n);

}

 
/*************************************************
* Function: HtoNs
* Description: ģ��htons�����������ֽ���ת�����ֽ���
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u16 AC_HtoNs(u16 h)
{

       // ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���

       // ������ΪС�ˣ�ת���ɴ���ٷ���

       return AC_checkCPUendian() ? h : BigLittleSwap16(h);

}

 
/*************************************************
* Function: NtoHs
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u16 AC_NtoHs(u16 n)
{

       // ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���

       // ������ΪС�ˣ���������ת����С���ٷ���

       return AC_checkCPUendian() ? n : BigLittleSwap16(n);

}

/*************************************************
* Function: AC_HtoNlu64
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u64 AC_HtoNlu64(u64 n)
{

       // ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���

       // ������ΪС�ˣ���������ת����С���ٷ���

       return AC_checkCPUendian() ? n : BigLittleSwap64(n);

}
