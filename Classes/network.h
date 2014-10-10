//
//  network.h
//  testServer_Mac
//  套接字处理函数头
//
//  Created by hENRYcHANG on 14-9-10.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#ifndef __testServer_Mac__network__
#define __testServer_Mac__network__

#include "def.h"
#include "structs.h"

#define dMAX_CLIENT_CNT                 200

//设置套接字为Noblocking模式的函数
void nonblock(SOCKET s);

//初始化服务器listen套接字的函数
SOCKET InitServerSock(int port,int backLog);

//新的连接请求的处理
void AcceptNewClient(SOCKET MotherSocket);

//断开连接
void DisconnectClient(sPCLIENT_DATA pClient);

//把传送的数据复制到缓冲器
void SendData(sPCLIENT_DATA pClient,const char *data,int size);

//给所有在连接中的客户端发送数据
void SendToAll(const char *data,int size);

//给游戏房内的所有玩家发送数据
void SendToRoom( sPROOM_DATA pRoom, const char *data,int size);
void SendToRoom2(sPROOM_DATA pRoom, sPCLIENT_DATA pClient, const char *data,int size);

//清空发送缓冲器
int FlushSendBuff(sPCLIENT_DATA pClient);

//recv
BOOL RecvFromClient(sPCLIENT_DATA pClient);

#endif /* defined(__testServer_Mac__network__) */

//EOF