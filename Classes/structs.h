//
//  structs.h
//  testServer_Mac
//  各种结构体头
//
//  Created by hENRYcHANG on 14-9-10.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#ifndef testServer_Mac_structs_h
#define testServer_Mac_structs_h

#include "def.h"

#define dMAX_SOCK_BUFF      1024
#define dNAME_LEN           50
#define dIP_LEN             20

struct client_data
{
    //用于数据传送的套接字等各种数据变量
    SOCKET          m_sock;
    
    char            m_Ip[dIP_LEN];
    
    char            m_recvBuff[dMAX_SOCK_BUFF];
    int             m_recvSize;
    int             m_recvPos;
    
    char            m_sendBuff[dMAX_SOCK_BUFF];
    int             m_sendSize;
    
    //对话名
    char            m_Name[dNAME_LEN];
    
    //用于连接状态确认的最后recv时间值
    DWORD           m_lastRecvTime;
    
    //用于链表连接的指针
    struct client_data *m_prev;
    struct client_data *m_next;
};

//方便使用struct client_data的typedef
typedef struct client_data sCLIENT_DATA,*sPCLIENT_DATA;

#endif

//EOF