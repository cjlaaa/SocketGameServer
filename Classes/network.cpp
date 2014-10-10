//
//  network.cpp
//  testServer_Mac
//  用于套接字处理的源程序文件
//
//  Created by hENRYcHANG on 14-9-10.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#include "utils.h"
#include "linked_list.h"
#include "network.h"
#include "structs.h"
#include "packet.h"
#include "message.h"
#include "player.h"
#include "room.h"

extern char g_Packet[dMAX_SEND_BUFF];
extern int  g_nPos;

//设置Win32用套接字为nonblock模式
#ifdef WIN32
void nonblock(SOCKET s)
{
    u_long u10n = 1L;

	ioctlsocket(s,FIONBIO,(unsigned long*)&u10n);
}
#else
//设置Linux,FreeBSD用套接字为nonblock模式
void nonblock(SOCKET s)
{
    int flags;
    
    flags = fcntl(s, F_GETFL,0);
    flags |= O_NONBLOCK;
    if (fcntl(s, F_SETFL,flags) < 0) log("* nonblock Error\r\n");
}
#endif

//当服务器listen套接字的初始化失败时,返回-1
SOCKET InitServerSock(int port,int backLog)
{
    struct sockaddr_in sa;
    SOCKET sock;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    
    if (sock < 0)
    {
        log("InitServerSock(), socket(..) failed [PORT:%d].. \r\n",port);
        return -1;
    }
    
    //设置成功,即使因错误而非正常结束,也能立刻用同样的地址进行bind
#ifndef WIN32
    int opt = 1;
    
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
    {
        log("* Error: setsockopt REUSEADDR\r\n");
        exit(1);
    }
#endif
    
    //LINGER的设定
    struct linger ld;
    ld.l_onoff = 0;
    ld.l_linger = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&ld, sizeof(ld)) < 0) log("* Error: setsockopt SO_LINGER...\r\n");
    
    //结构体初始化
    memset((char *)&sa, 0, sizeof(sa));
    sa.sin_family = PF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;
    
    //与地址绑定
    if (bind(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0)
    {
        log("InitServerSock(), bind(..) failed.. [PORT:%d]\r\n",port);
        closesocket(sock);
        return -1;
    }
    
    //设置以创建的套接字为Nonblocking模式
    nonblock(sock);
    
    //设置套接字处于等待状态
    listen(sock,backLog);
    
    return sock;
}

//新的连接请求的处理
void AcceptNewClient(SOCKET MotherSocket)
{
    SOCKET newDesc;
    struct sockaddr_in peer;
#if defined(WIN32)
    int peersize;
#else 
    socklen_t peersize;
#endif
    
    peersize = sizeof(peer);
    
    newDesc = accept(MotherSocket, (struct sockaddr*)&peer, &peersize);
    
    if (newDesc < 0)
    {
        log("Accept New Client Failed!\r\n");
        return;
    }
    
    //socket nonblocking
    nonblock(newDesc);
    
    if (g_PLAYERS.m_totalPlayerCnt == dMAX_CLIENT_CNT)
    {
        g_nPos = 2;
        PutWord(g_Packet,PACKET_SERVERISFULL,g_nPos);
        PutSize(g_Packet, g_nPos);
        
        send(newDesc,g_Packet,g_nPos,0);
        
        closesocket(newDesc);
        return;
    }
    
    sPCLIENT_DATA newClient = PLAYER_NewData();
    if (!newClient)
    {
        closesocket(newDesc);
        return;
    }
    
    //客户端数据的初始化
    PLAYER_InitPlayerData(newClient,newDesc,(char*)inet_ntoa(peer.sin_addr));
    
    //链表连接
    INSERT_TO_LIST(g_PLAYERS.m_ClientList,newClient,m_prev,m_next);
    g_PLAYERS.m_totalPlayerCnt++;
    
    log("Accept New Connection: %d [%s]\r\n",newDesc,newClient->m_Ip);
}

//把传送的数据复制到缓冲器
void SendData(sPCLIENT_DATA pClient,const char *data,int size)
{
    if ((pClient->m_sendSize + size) > dMAX_SEND_BUFF) return;
    
    memcpy(&pClient->m_sendBuff[pClient->m_sendSize], data, size);
    pClient->m_sendSize += size;
}

//给所有在连接中的客户端发送数据
void SendToAll(const char *data,int size)
{
    sPCLIENT_DATA client,next_client;
    
    LIST_WHILE(g_PLAYERS.m_ClientList, client, next_client, m_next);
    SendData(client, data, size);
    LIST_WHILEEND(g_PLAYERS.m_ClientList, client, next_client);
}

void SendToRoom(sPROOM_DATA pRoom,const char *data,int size)
{
    if (!pRoom)
    {
        log("SendToRoom: !pRoom\r\n");
        return;
    }
    
    sPCLIENT_DATA client,next_client;
    
    LIST_WHILE(pRoom->m_inPlayer, client, next_client, m_game_next);
    SendData(client, data, size);
    LIST_WHILEEND(pRoom->m_inPlayer, client, next_client);
}

void SendToRoom2(sPROOM_DATA pRoom,sPCLIENT_DATA pClient,const char* data,int size)
{
    if (!pRoom)
    {
        log("SendToRoom: !pRoom\r\n");
        return;
    }
    
    sPCLIENT_DATA client,next_client;
    
    LIST_WHILE(pRoom->m_inPlayer, client, next_client, m_game_next);
    if(pClient!=client) SendData(client, data, size);
    LIST_WHILEEND(pRoom->m_inPlayer, client, next_client);
}

//清空发送缓冲器
int FlushSendBuff(sPCLIENT_DATA pClient)
{
    int sendSize;
    
    sendSize = send(pClient->m_sock,pClient->m_sendBuff,pClient->m_sendSize,0);
    
    //Error
    if (sendSize<=0) return -1;
    
    //如果已经发送的数据比应该发送的数据少,则只处理已发送的数据
    if (sendSize < pClient->m_sendSize)
    {
        memmove(&pClient->m_sendBuff[0],
                &pClient->m_sendBuff[sendSize],
                pClient->m_sendSize - sendSize);
        
        pClient->m_sendSize -= sendSize;
    }
    else
    {
        *pClient->m_sendBuff = '\0';
        pClient->m_sendSize = 0;
    }
    return sendSize;
}

//断开连接
void DisconnectClient(sPCLIENT_DATA pClient)
{
    //当已经进入游戏房内时
    if (GET_PLAYER_STATE(pClient)==PLAYER_STATE_INROOM)
    {
        ROOM_LeavePlayer(pClient,dLEAVEPLAYER_CONCLOSE);
    }
    
    //若状态值比PLAYER_STATE_WAITROOM大,则表示在g_PLAYER.m_IdList和g_PLAYERS.m_NameList中存在数据,因此从列表中删除.
    if (GET_PLAYER_STATE(pClient) >= PLAYER_STATE_WAITROOM)
    {
        int hashIndex = GetStrHashIndex(pClient->m_Player.m_id);
        REMOVE_FROM_LIST(g_PLAYERS.m_IdList[hashIndex], pClient, m_id_prev, m_id_next);
        
        hashIndex = GetStrHashIndex(pClient->m_Player.m_name);
        REMOVE_FROM_LIST(g_PLAYERS.m_NameList[hashIndex], pClient, m_wait_prev, m_wait_next);
    }
    
    if (GET_PLAYER_STATE(pClient)==PLAYER_STATE_WAITROOM)
    {
        REMOVE_FROM_LIST(g_PLAYERS.m_InWaitRoomList, pClient, m_wait_prev, m_wait_next);
    }
    
    log("Connection End: %d [%s]\r\n",pClient->m_sock,pClient->m_Ip);
    REMOVE_FROM_LIST(g_PLAYERS.m_ClientList, pClient, m_prev, m_next);
    if (pClient->m_sock!=INVALID_SOCKET) closesocket(pClient->m_sock);
    
    g_PLAYERS.m_totalPlayerCnt--;
    PLAYER_DeleteData(pClient);
}

//recv
BOOL RecvFromClient(sPCLIENT_DATA pClient)
{
    int recvSize;
    char recvBuff[dMAX_RECV_BUFF];
    
    recvSize = recv(pClient->m_sock,recvBuff,1024,0);
    
    if (recvSize==0) return 0;
    
    if (recvSize < 0)
    {
        //出现除Nonblock以外的错误时,结束连接状态
#if defined(WIN32)
		if (WSAGetLastError()!=WSAEWOULDBLOCK)return 0;
#else
        if (errno!=EWOULDBLOCK) return 0;
#endif
        else return 1;
    }
    
    //Buffer Overflow
    if((pClient->m_recvSize + recvSize)>=dMAX_RECV_BUFF) return 0;
    
    pClient->m_lastRecvTime = timeGetTime();
    
    memcpy(&pClient->m_recvBuff[pClient->m_recvSize], recvBuff, recvSize);
    pClient->m_recvSize += recvSize;
    
    return 1;
}

//EOF