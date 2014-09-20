//
//  packet.h
//  testServer_Mac
//  数据包处理函数的头
//
//  Created by hENRYcHANG on 14-9-10.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#ifndef __testServer_Mac__packet__
#define __testServer_Mac__packet__

#include "def.h"

//在pPacket参数的nPos位置放置BYTE类型的value值
void PutByte(char *pPacket,BYTE value,int &nPos);

//把位于pPacket参数nPos位置的值以BYTE类型返回
BYTE GetByte(char *pPacket,int &nPos);

//在pPacket参数的nPos位置放置WORD类型的value值
void PutWord(char *pPacket,WORD value,int &nPos);

//把位于pPacket参数nPos位置的值以WORD类型返回
WORD GetWord(char *pPacket,int &nPos);

//在pPacket参数的nPos位置放置DWORD类型的value值
void PutDword(char *pPacket,DWORD value,int &nPos);

//把位于pPacket参数nPos位置的值以DWORD类型返回
DWORD GetDword(char *pPacket,int &nPos);

//在pPacket参数的nPos位置放置int类型的value值
void PutInteger(char *pPacket,int value,int &nPos);

//把位于pPacket参数nPos位置的值以int类型返回
int GetInteger(char *pPacket,int &nPos);

//在pPacket参数的nPos位置放置short类型的value值
void PutShort(char *pPacket,short value,int &nPos);

//把位于pPacket参数nPos位置的值以short类型返回
short GetShort(char *pPacket,int &nPos);

//在pPacket参数的nPos位置放置字符串
void PutString(char *pPacket,char* str,int &nPos);

//把位于pPacket参数nPos位置的字符串数据复制到pBuffer(包括NULL的处理)
void GetString(char *pPacket,char* pBuffer,int &nPos);

//设置数据包的总长度
void PutSize(char *pPacket,WORD nPos);

#endif /* defined(__testServer_Mac__packet__) */

//EOF
