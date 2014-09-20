//
//  message.cpp
//  testServer_Mac
//  处理接受的数据包的函数
//
//  Created by hENRYcHANG on 14-9-10.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#include "message.h"
#include "packet.h"
#include "utils.h"
#include "network.h"

//维持连接
void PACKET_AliveCheck(sPCLIENT_DATA pClient)
{
}

//修改绰号
void PACKET_ChangeNick(sPCLIENT_DATA pClient)
{
    char nick[64];
    
    GetString(pClient->m_recvBuff,nick,pClient->m_recvPos);
    
    strcpy(pClient->m_Name,nick);
}

//聊天消息处理
void PACKET_RecvMsg(sPCLIENT_DATA pClient)
{
    char msg[dMAX_SOCK_BUFF];
    
    GetString(pClient->m_recvBuff, msg, pClient->m_recvPos);
    
    int packetPos;
    char packet[dMAX_SOCK_BUFF];
    char tmp[dMAX_SOCK_BUFF];
    
    sprintf(tmp, "%s: %s\r\n",pClient->m_Name,msg);
    log(tmp);
    
    packetPos = 2;
    PutWord(packet, dPACKET_SEND_MSG, packetPos);
    PutString(packet, tmp, packetPos);
    PutSize(packet, packetPos);
    
    SendToAll(packet,packetPos);
}

//数据处理
BOOL ReadRecvBuff(sPCLIENT_DATA pClient)
{
    WORD msgSize;
    WORD tag;
    
    while (pClient->m_recvSize>0)
    {
        pClient->m_recvPos = 0;
        
        //前两个字符为大小
        msgSize = GetWord(pClient->m_recvBuff, pClient->m_recvPos);
        //第三个字符为tag
        tag = GetWord(pClient->m_recvBuff, pClient->m_recvPos);
        
        //当目前m_recvSize内数据的长度小于数据包总长度时
        if (pClient->m_recvSize < msgSize) return 1;
        
        //数据包处理
        switch (tag)
        {
            case dPACKET_ALIVE_CHECK:
                PACKET_AliveCheck(pClient);
                break;
            case dPACKET_CHANGE_NICK:
                PACKET_ChangeNick(pClient);
                break;
            case dPACKET_RECV_MSG:
                PACKET_RecvMsg(pClient);
                break;
            case dPACKET_SEND_MSG:
                break;
            default:
                log("Error Msg : %d",tag);
                break;
        }
        
        //?
        memmove(&pClient->m_recvBuff[0],
                &pClient->m_recvBuff[msgSize],
                pClient->m_recvSize - msgSize);
        
        pClient->m_recvSize -= msgSize;
    }
    return 1;
}

//EOF