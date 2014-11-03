//
//  player.cpp
//  testServer_Mac
//
//  Created by hENRYcHANG on 14-10-1.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#include "player.h"

//在Garbage List中插入用户的数据
void PLAYER_DeleteData(sPCLIENT_DATA pData)
{
    
}

//如果在Garbage List中存在数据,则重新使用在Garbage List的数据,否则通过malloc函数分配新的内存.
sPCLIENT_DATA PLAYER_NewData()
{
    return sPCLIENT_DATA();
}

//初始化新连接客户数据
void PLAYER_InitPlayerData(sPCLIENT_DATA pClient,SOCKET sock,char *ip)
{
    
}

//口令确认函数
//return 0 = 口令不对
//return 1 = 口令对
BOOL PLAYER_CheckValidPassword(sPCLIENT_DATA pClient,char *pw)
{
    //TODO:注册时检查ID和口令的函数,一般使用基于SQL的DBMS.
    return FALSE;
}

//玩家数据的装入
//return 0 = 读取数据失败
//return 1 = 读取数据成功
BOOL PLAYER_LoadPlayerData(sPCLIENT_DATA pClient)
{
    //TODO:读进玩家数据的函数
    
    //临时
    sprintf(pClient->m_Player.m_name, "client%d",pClient->m_sock);
    
    return 1;
}

//存储玩家的数据
void PLAYER_SavePlayerData()
{
    //TODO:存储玩家数据的函数
}

//当创建完新入玩家时,再创建玩家的信息数据
void PLAYER_CreateNewPlayerData(sPCLIENT_DATA pClient)
{
    
}

//创建新入玩家时,检查其名是否已经被占用
//return 0 = 未被占用
//return 1 = 已被占用
BOOL PLAYER_CheckUsedName(char *name)
{
    return FALSE;
}

//确认是否是已经连接的玩家
BOOL PLAYER_CheckConnectedID(char *id)
{
    return 0;
}

//以ID查找玩家数据
sPCLIENT_DATA PLAYER_FindPlayerByID(char *id)
{
    return sPCLIENT_DATA();
}

//以绰号查找玩家数据
sPCLIENT_DATA PLAYER_FindPlayerByName(char *name)
{
    return sPCLIENT_DATA();
}

//查找房间里的用户
sPCLIENT_DATA PLAYER_FindPlayerInRoom(sPROOM_DATA pRoom,char* name)
{
    return sPCLIENT_DATA();
}

//EOF