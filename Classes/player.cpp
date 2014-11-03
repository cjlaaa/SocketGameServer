//
//  player.cpp
//  testServer_Mac
//
//  Created by hENRYcHANG on 14-10-1.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#include "player.h"
#include "linked_list.h"
#include "utils.h"

//在Garbage List中插入用户的数据
void PLAYER_DeleteData(sPCLIENT_DATA pData)
{
    INSERT_TO_LIST(g_PLAYERS.m_ClientMemoryList, pData, m_prev, m_next);
    g_PLAYERS.m_totalMemoryCnt++;
}

//如果在Garbage List中存在数据,则重新使用在Garbage List的数据,否则通过malloc函数分配新的内存.
sPCLIENT_DATA PLAYER_NewData()
{
    sPCLIENT_DATA pData = NULL;
    
    //若在Garbage List中有数据
    if (g_PLAYERS.m_ClientMemoryList)
    {
        pData = g_PLAYERS.m_ClientMemoryList;
        
        REMOVE_FROM_LIST(g_PLAYERS.m_ClientMemoryList, pData, m_prev, m_next);
        
        g_PLAYERS.m_totalMemoryCnt--;
        return pData;
    }
    else
    {
        pData = (sPCLIENT_DATA)malloc(sizeof(sCLIENT_DATA));
        
        if (!pData)
            return NULL;
        return pData;
    }
}

//初始化新连接客户数据
void PLAYER_InitPlayerData(sPCLIENT_DATA pClient,SOCKET sock,char *ip)
{
    //用于套接字数据传送的变量
    pClient->m_sock = sock;
    
    strcpy(pClient->m_Ip,ip);
    
    *pClient->m_recvBuff = '\0';
    pClient->m_recvSize = 0;
    pClient->m_recvPos = 0;
    
    *pClient->m_sendBuff = '\0';
    pClient->m_sendSize = 0;
    
    //用于确认连接状态的最后recv时间值
    pClient->m_lastRecvTime = timeGetTime();
    //用于防止攻击
    pClient->m_lastCheckTime = timeGetTime();
    pClient->m_isBadConnection = 0;
    
    //玩家数据结构体
    *pClient->m_Player.m_id = '\0';
    *pClient->m_Player.m_name = '\0';
    pClient->m_Player.m_money = 0;
    pClient->m_Player.m_grade = 0;
    
    //房间数据结构体指针
    pClient->m_pRoom = NULL;
    
    //客户状态值
    pClient->m_State = PLAYER_STATE_LOGIN;
    
    //用于链表连接的指针
    pClient->m_prev = NULL;
    pClient->m_next = NULL;
    
    pClient->m_game_prev = NULL;
    pClient->m_game_next = NULL;
    
    pClient->m_wait_prev = NULL;
    pClient->m_wait_next = NULL;
    
    pClient->m_name_prev = NULL;
    pClient->m_name_next = NULL;
    
    pClient->m_id_prev = NULL;
    pClient->m_id_next = NULL;
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
    return 0;
}

//确认是否是已经连接的玩家
BOOL PLAYER_CheckConnectedID(char *id)
{
    int hashIndex = GetStrHashIndex(id);
    
    sPCLIENT_DATA client ,next_client;
    
    LIST_WHILE(g_PLAYERS.m_IdList[hashIndex], client, next_client, m_id_next);
    
    if (!strcmp(client->m_Player.m_id, id))
        return 1;
    
    LIST_WHILEEND(g_PLAYERS.m_IdList[hashIndex], client, next_client);
    
    return 0;
}

//以ID查找玩家数据
sPCLIENT_DATA PLAYER_FindPlayerByID(char *id)
{
    int hashIndex = GetStrHashIndex(id);
    
    sPCLIENT_DATA client ,next_client;
    
    LIST_WHILE(g_PLAYERS.m_IdList[hashIndex], client, next_client, m_id_next);
    
    if (!strcmp(client->m_Player.m_id, id))
        return client;
    
    LIST_WHILEEND(g_PLAYERS.m_IdList[hashIndex], client, next_client);
    
    return NULL;
}

//以绰号查找玩家数据
sPCLIENT_DATA PLAYER_FindPlayerByName(char *name)
{
    int hashIndex = GetStrHashIndex(name);
    
    sPCLIENT_DATA client ,next_client;
    
    LIST_WHILE(g_PLAYERS.m_NameList[hashIndex], client, next_client, m_name_next);
    
    if (!strcmp(client->m_Player.m_name, name))
        return client;
    
    LIST_WHILEEND(g_PLAYERS.m_NameList[hashIndex], client, next_client);
    
    return NULL;
}

//查找房间里的用户
sPCLIENT_DATA PLAYER_FindPlayerInRoom(sPROOM_DATA pRoom,char* name)
{
    sPCLIENT_DATA client ,next_client;
    
    LIST_WHILE(pRoom->m_inPlayer, client, next_client, m_game_next);
    
    if (!strcmp(client->m_Player.m_name, name))
        return client;
    
    LIST_WHILEEND(pRoom->m_inPlayer, client, next_client);
    
    return NULL;
}

//EOF