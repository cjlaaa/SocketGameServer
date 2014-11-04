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
#include "packet.h"
#include "player.h"
#include "linked_list.h"
#include "room.h"

extern char g_Packet[dMAX_SEND_BUFF];
extern int g_nPos;

//当从服务器向客户端发送的数据包回到服务器时,需要使用函数指针数据,为此,为了处理例外情况,需要如下所示的函数
BOOL PACKET_Null(sPCLIENT_DATA pClient)
{
    return TRUE;
}

//为了防止通过timeGetTime等时间函数的Hooking,加速时间的Speedhack类攻击工具的使用,而进行的数据包的处理.
BOOL PACKET_ConnectionCheck(sPCLIENT_DATA pClient)
{
    DWORD currTime = timeGetTime();
    
    if (currTime < pClient->m_lastCheckTime + dCHECK_SPEEDHACK_TIME)
    {
        //如果确定了一次以上的数据包,比dCHECK_sPEEDHACK_TIME,快到达时,断开连接.
        if (pClient->m_isBadConnection)
        {
            g_nPos = 2;
            PutWord(g_Packet, PACKET_BAD_CONNECTION, g_nPos);
            PutSize(g_Packet, g_nPos);
            SendData(pClient, g_Packet, g_nPos);
            return FALSE;
        }
        else
        {
            pClient->m_lastCheckTime = currTime;
            pClient->m_isBadConnection = 1;
        }
    }
    else
    {
        pClient->m_lastCheckTime = currTime;
        pClient->m_isBadConnection = 0;
    }
    return TRUE;
}

//注册处理
BOOL PACKET_LoginInfo(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient)!=PLAYER_STATE_LOGIN)
        return 1;
    
    char id[512];
    char pw[512];
    
    GetString(pClient->m_recvBuff, id, pClient->m_recvPos);
    GetString(pClient->m_recvBuff, pw, pClient->m_recvPos);
    
    //当客户发送的ID的长度超过数据结构体中存储ID的变量的长度是时,断开连接.
    if (strlen(id) > dID_LEN)
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_LOGIN_RESULT, g_nPos);
        PutByte(g_Packet, dLOGIN_RES_EXCEPTION, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    //确认是不是已经连接中的ID
    if (PLAYER_CheckConnectedID(id))
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_LOGIN_RESULT, g_nPos);
        PutByte(g_Packet, dLOGIN_RES_USEDID, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    strcpy(GET_PLAYER_ID(pClient), id);
    
    //当口令错或没有对应ID时
    if (!PLAYER_CheckValidPassword(pClient, pw))
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_LOGIN_RESULT, g_nPos);
        PutByte(g_Packet, dLOGIN_RES_BADPW, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
    }
    else
    {
        //注册成功
        g_nPos = 2;
        PutWord(g_Packet, PACKET_LOGIN_RESULT, g_nPos);
        PutByte(g_Packet, dLOGIN_RES_SUCCESS, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        GET_PLAYER_STATE(pClient) = PLAYER_STATE_DATAREADY;
        
        int hashIndex = GetStrHashIndex(pClient->m_Player.m_id);
        
        INSERT_TO_LIST(g_PLAYERS.m_IdList[hashIndex], pClient, m_id_prev, m_id_next);
    }
    
    return 1;
}

//客户端的玩家数据传送请求
BOOL PACKET_GetPlayerInfo(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient)!=PLAYER_STATE_DATAREADY)
        return 1;
    
    //当玩家数据不存在时
    if (!PLAYER_LoadPlayerData(pClient))
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_NULL_PLAYER, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
    
        GET_PLAYER_STATE(pClient) = PLAYER_STATE_CREATEPLAYER;
        
        return 1;
    }
    else
    {
        g_nPos = 2;
        
        PutWord(g_Packet, PACKET_SEND_PLAYERINFO, g_nPos);
        PutString(g_Packet, pClient->m_Player.m_id, g_nPos);
        PutString(g_Packet, pClient->m_Player.m_name, g_nPos);
        PutInteger(g_Packet, pClient->m_Player.m_grade, g_nPos);
        PutInteger(g_Packet, pClient->m_Player.m_money, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        GET_PLAYER_STATE(pClient) = PLAYER_STATE_WAITROOM;
        
        INSERT_TO_LIST(g_PLAYERS.m_InWaitRoomList, pClient, m_wait_prev, m_wait_next);
        
        //Hash列表处理
        int hashIndex = GetStrHashIndex(pClient->m_Player.m_name);
        
        INSERT_TO_LIST(g_PLAYERS.m_NameList[hashIndex], pClient, m_name_prev, m_name_next);
    }
    return 1;
}

//新入玩家数据的创建
BOOL PACKET_CreatePlayer(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient)!=PLAYER_STATE_CREATEPLAYER)
        return 1;
    
    char name[128];
    GetString(pClient->m_recvBuff, name, pClient->m_recvPos);
    
    //例外处理
    if (strlen(name) > dNAME_LEN)
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_CREATE_PLAYER_RESULT, g_nPos);
        PutByte(g_Packet, dCREATE_PLAYER_RES_EXCEPTION, g_nPos);
        PutSize(g_Packet, g_nPos);
        //SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    //当指定的绰号是其他玩家已经使用的名字时
    if (PLAYER_CheckUsedName(name))
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_CREATE_PLAYER_RESULT, g_nPos);
        PutByte(g_Packet, dCREATE_PLAYER_RES_USEDNAME, g_nPos);
        PutSize(g_Packet, g_nPos);
        //SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    //将接收的信息复制到数据结构体
    strcpy(pClient->m_Player.m_name, name);
    
    PLAYER_CreateNewPlayerData(pClient);
    
    g_nPos = 2;
    PutWord(g_Packet, PACKET_CREATE_PLAYER_RESULT, g_nPos);
    PutByte(g_Packet, dCREATE_PLAYER_RES_SUCCESS, g_nPos);
    PutSize(g_Packet, g_nPos);
    //SendData(pClient, g_Packet, g_nPos);
    
    GET_PLAYER_STATE(pClient) = PLAYER_STATE_DATAREADY;
    return 1;
}

//等候室中的聊天
BOOL PACKET_WaitRoomClientChat(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient)!=PLAYER_STATE_WAITROOM)
        return 1;
    
    char str[512];
    GetString(pClient->m_recvBuff,str, pClient->m_recvPos);
    
    g_nPos = 2;
    
    PutWord(g_Packet, PACKET_WAITROOM_SERVER_CHAT, g_nPos);
    PutString(g_Packet, pClient->m_Player.m_name, g_nPos);
    PutString(g_Packet, str, g_nPos);
    PutSize(g_Packet, g_nPos);
    
    sPCLIENT_DATA client,next_client;
    
    LIST_WHILE(g_PLAYERS.m_InWaitRoomList, client, next_client, m_wait_next);
    SendData(client, g_Packet, g_nPos);
    LIST_WHILEEND(g_PLAYERS.m_InWaitRoomList, client, next_client);
    
    return 1;
}

//私聊处理
BOOL PACKET_WhisperMsg(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient) < PLAYER_STATE_WAITROOM)
        return 1;
    
    char to_player[128];
    char str[512];
    GetString(pClient->m_recvBuff, to_player, pClient->m_recvPos);
    GetString(pClient->m_recvBuff, str, pClient->m_recvPos);
    sPCLIENT_DATA pToPlayer = PLAYER_FindPlayerByName(to_player);
    
    //找不到私聊对象
    if (!pToPlayer)
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_WHISPER_SENDMSG, g_nPos);
        PutString(g_Packet, " ", g_nPos);
        PutString(g_Packet, "找不到对象.", g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    g_nPos = 2;
    PutWord(g_Packet, PACKET_WHISPER_SENDMSG, g_nPos);
    PutString(g_Packet, pClient->m_Player.m_name, g_nPos);
    PutString(g_Packet, str, g_nPos);
    PutSize(g_Packet, g_nPos);
    SendData(pToPlayer, g_Packet, g_nPos);
    SendData(pClient, g_Packet, g_nPos);
    
    return 1;
}

//创建新的房间
BOOL PACKET_CreateGameRoom(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient) != PLAYER_STATE_WAITROOM)
        return 1;
    
    //已经达到可创建房间最大数时
    if (g_ROOMS.m_RoomCnt>=dMAX_ROOM_CNT)
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_CREAT_GAMEROOM_RES, g_nPos);
        PutByte(g_Packet, dCREATE_ROOM_RES_FULL, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    char roomName[512];
    char roomPassword[512];
    BYTE roomType;
    BYTE maxInPlayer;
    
    GetString(pClient->m_recvBuff, roomName, pClient->m_recvPos);
    maxInPlayer = GetByte(pClient->m_recvBuff, pClient->m_recvPos);
    roomType = GetByte(pClient->m_recvBuff, pClient->m_recvPos);
    
    //例外处理
    if (strlen(roomName) > dROOM_NAME_LEN)
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_CREAT_GAMEROOM_RES, g_nPos);
        PutByte(g_Packet, dCREATE_ROOM_RES_EXCEPTION, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    //当创建的房间是秘密房间时
    if (roomType==dROOM_TYPE_PRIVATE)
    {
        GetString(pClient->m_recvBuff, roomPassword, pClient->m_recvPos);
        
        //例外处理
        if (strlen(roomPassword) > dROOM_PASSWORD_LEN)
        {
            g_nPos = 2;
            PutWord(g_Packet, PACKET_CREAT_GAMEROOM_RES, g_nPos);
            PutByte(g_Packet, dCREATE_ROOM_RES_EXCEPTION, g_nPos);
            PutSize(g_Packet, g_nPos);
            SendData(pClient, g_Packet, g_nPos);
            
            return 1;
        }
    }
    
    //获取空房间的编号
    int roomNum = ROOM_GetEmptyArray();
    
    //例外处理
    if (roomNum==-1)
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_CREAT_GAMEROOM_RES, g_nPos);
        PutByte(g_Packet, dCREATE_ROOM_RES_EXCEPTION, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    //分配内存
    sPROOM_DATA pNewRoom = ROOM_NewData();
    if (!pNewRoom)
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_CREAT_GAMEROOM_RES, g_nPos);
        PutByte(g_Packet, dCREATE_ROOM_RES_EXCEPTION, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    //初始化房间数据结构体
    ROOM_InitRoomData(pNewRoom);
    pNewRoom->m_roomNum = roomNum;
    strcpy(pNewRoom->m_roomName, roomName);
    pNewRoom->m_roomType = roomType;
    pNewRoom->m_MaxPlayer = maxInPlayer;
    if (roomType==dROOM_TYPE_PRIVATE)
        strcpy(pNewRoom->m_password, roomPassword);
    else
        *pNewRoom->m_password = '\0';
    pNewRoom->m_roomState = dROOM_STATE_WAIT;
    
    //创建房间的玩家成为初期房主
    pNewRoom->m_roomOwner = pClient;
    
    //在房间玩家列表中追加玩家
    INSERT_TO_LIST(pNewRoom->m_inPlayer, pClient, m_game_prev, m_game_next);
    pNewRoom->m_inPlayerCnt++;
    
    //在总房间列表中追加新创建的房间
    INSERT_TO_LIST(g_ROOMS.m_RoomList, pNewRoom, m_prev, m_next);
  
    
    g_ROOMS.m_roomArray[roomNum] = pNewRoom;
    g_ROOMS.m_RoomCnt++;
    
    //在等待者列表中删除
    REMOVE_FROM_LIST(g_PLAYERS.m_InWaitRoomList, pClient, m_wait_prev, m_wait_next);
    
    //为了快速查找玩家所属房间,连接房间指针
    pClient->m_pRoom = pNewRoom;
    GET_PLAYER_STATE(pClient) = PLAYER_STATE_INROOM;
    
    //房间创建成功
    g_nPos = 2;
    PutWord(g_Packet, PACKET_CREAT_GAMEROOM_RES, g_nPos);
    PutByte(g_Packet, dCREATE_ROOM_RES_SUCCESS, g_nPos);
    PutSize(g_Packet, g_nPos);
    SendData(pClient, g_Packet, g_nPos);
    
    return 1;
}

//传送创建的房间列表
BOOL PACKET_GetRoomList(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient) != PLAYER_STATE_WAITROOM)
        return 1;
    
    g_nPos = 2;
    PutWord(g_Packet, PACKET_SEND_ROOMLIST, g_nPos);
    PutWord(g_Packet, g_ROOMS.m_RoomCnt, g_nPos);
    
    sPROOM_DATA room,next_room;
    
    LIST_WHILE(g_ROOMS.m_RoomList, room, next_room, m_next);
    PutWord(g_Packet, room->m_roomNum, g_nPos);
    PutString(g_Packet, room->m_roomName, g_nPos);
    PutByte(g_Packet, room->m_roomType, g_nPos);
    PutByte(g_Packet, room->m_roomState, g_nPos);
    PutByte(g_Packet, room->m_inPlayerCnt, g_nPos);
    PutByte(g_Packet, room->m_MaxPlayer, g_nPos);
    LIST_WHILEEND(g_ROOMS.m_RoomList, room, next_room);
    PutSize(g_Packet, g_nPos);
    SendData(pClient, g_Packet, g_nPos);
    
    return 1;
}

//要进入所创建房间的客户请求
BOOL PACKET_EnterGameRoom(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient) != PLAYER_STATE_WAITROOM)
        return 1;
    
    WORD roomNum;
    char password[128];
    
    roomNum = GetWord(pClient->m_recvBuff, pClient->m_recvPos);
    GetString(pClient->m_recvBuff, password, pClient->m_recvPos);
    
    //例外处理
    if (roomNum >= dMAX_ROOM_CNT)
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_ENTER_GAMEROOM_RES, g_nPos);
        PutByte(g_Packet, dENTER_GAMEROOM_RES_EXCEPTION, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    sPROOM_DATA pRoom = g_ROOMS.m_roomArray[roomNum];
    //当不是所创建的房间时
    if (!pRoom)
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_ENTER_GAMEROOM_RES, g_nPos);
        PutByte(g_Packet, dENTER_GAMEROOM_RES_CANTFIND, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    //例外处理
    if (!pRoom->m_roomOwner)
    {
        log("PACKET_EnterGameRoom : !pRoom->m_roomOwner\r\n");
        
        g_nPos = 2;
        PutWord(g_Packet, PACKET_ENTER_GAMEROOM_RES, g_nPos);
        PutByte(g_Packet, dENTER_GAMEROOM_RES_EXCEPTION, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    //可入场玩家数已满
    if (pRoom->m_inPlayerCnt==pRoom->m_MaxPlayer)
    {
        g_nPos = 2;
        PutWord(g_Packet, PACKET_ENTER_GAMEROOM_RES, g_nPos);
        PutByte(g_Packet, dENTER_GAMEROOM_RES_FULL, g_nPos);
        PutSize(g_Packet, g_nPos);
        SendData(pClient, g_Packet, g_nPos);
        
        return 1;
    }
    
    //如果是秘密房间,则比较口令
    if (pRoom->m_roomType==dROOM_TYPE_PRIVATE)
    {
        if (strcmp(pRoom->m_password, password))
        {
            g_nPos = 2;
            PutWord(g_Packet, PACKET_ENTER_GAMEROOM_RES, g_nPos);
            PutByte(g_Packet, dENTER_GAMEROOM_RES_BADPASS, g_nPos);
            PutSize(g_Packet, g_nPos);
            SendData(pClient, g_Packet, g_nPos);
            
            return 1;
        }
    }
    
    //进入房间成功
    g_nPos = 2;
    PutWord(g_Packet, PACKET_ENTER_GAMEROOM_RES, g_nPos);
    PutByte(g_Packet, dENTER_GAMEROOM_RES_SUCCESS, g_nPos);
    PutSize(g_Packet, g_nPos);
    SendData(pClient, g_Packet, g_nPos);
    
    //给新进入的玩家传送已经在房间内的玩家的信息
    g_nPos = 2;
    PutWord(g_Packet, PACKET_INPLAYER_INFO, g_nPos);
    PutByte(g_Packet, pRoom->m_inPlayerCnt, g_nPos);
    sPCLIENT_DATA client,next_client;
    LIST_WHILE(pRoom->m_inPlayer, client, next_client, m_game_next);
    PutString(g_Packet, client->m_Player.m_name, g_nPos);
    PutInteger(g_Packet, client->m_Player.m_grade, g_nPos);
    PutInteger(g_Packet, client->m_Player.m_money, g_nPos);
    LIST_WHILEEND(pRoom->m_inPlayer, client, next_client);
    PutSize(g_Packet, g_nPos);
    SendData(pClient, g_Packet, g_nPos);
    
    //给所有已经在房间内的玩家床送新进入房间的玩家的信息
    g_nPos = 2;
    PutWord(g_Packet, PACKET_ENTER_PLAYER_INFO, g_nPos);
    PutString(g_Packet, pClient->m_Player.m_name, g_nPos);
    PutInteger(g_Packet, pClient->m_Player.m_grade, g_nPos);
    PutInteger(g_Packet, pClient->m_Player.m_money, g_nPos);
    PutSize(g_Packet, g_nPos);
    SendToRoom(pRoom, g_Packet, g_nPos);
    
    //通知谁是版主
    g_nPos = 2;
    PutWord(g_Packet, PACKET_ROOM_OWNER_INFO, g_nPos);
    PutString(g_Packet, pRoom->m_roomOwner->m_Player.m_name, g_nPos);
    PutSize(g_Packet, g_nPos);
    SendData(pClient, g_Packet, g_nPos);
    
    //在房间用户列表中,插入新入玩家的数据
    INSERT_TO_LIST(pRoom->m_inPlayer, pClient, m_game_prev, m_game_next);
    pRoom->m_inPlayerCnt++;
    
    //从等待者列表中删除
    REMOVE_FROM_LIST(g_PLAYERS.m_InWaitRoomList, pClient, m_wait_prev, m_wait_next);
    
    //为了快速查找玩家所在的房间,连接房间数据指针
    pClient->m_pRoom = pRoom;
    
    GET_PLAYER_STATE(pClient) = PLAYER_STATE_INROOM;
    
    return 1;
}

//离开房间
BOOL PACKET_LeaveGameRoom(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient) != PLAYER_STATE_INROOM)
        return 1;
    
    ROOM_LeavePlayer(pClient, dLEAVEPLAYER_NORMAL);
    
    return 1;
}

//在房间内的聊天
BOOL PACKET_GameRoomClientChat(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient) != PLAYER_STATE_INROOM)
        return 1;
    
    char str[512];
    GetString(pClient->m_recvBuff, str, pClient->m_recvPos);
    
    g_nPos = 2;
    PutWord(g_Packet, PACKET_GAMEROOM_SERVER_CHAT, g_nPos);
    PutString(g_Packet, pClient->m_Player.m_name, g_nPos);
    PutString(g_Packet, str, g_nPos);
    PutSize(g_Packet, g_nPos);
    SendToRoom(pClient->m_pRoom, g_Packet, g_nPos);
    
    return 1;
}

//剔除玩家
BOOL PACKET_DesportPlayer(sPCLIENT_DATA pClient)
{
    if (GET_PLAYER_STATE(pClient) != PLAYER_STATE_INROOM)
        return 1;
    
    char desportUser[50];
    GetString(pClient->m_recvBuff, desportUser, pClient->m_recvPos);
    
    sPROOM_DATA pRoom = pClient->m_pRoom;
    if (!pRoom)
    {
        log("PACKET_DesportPlayer : !pRoom\r\n");
        return 1;
    }
    
    if (pRoom->m_roomOwner!=pClient)
        return 1;
    
    sPCLIENT_DATA pTarget = PLAYER_FindPlayerInRoom(pRoom, desportUser);
    if (!pTarget)
        return 1;
    
    g_nPos = 2;
    PutWord(g_Packet, PACKET_DESPORT_DONE, g_nPos);
    PutString(g_Packet, pTarget->m_Player.m_name, g_nPos);
    PutSize(g_Packet, g_nPos);
    SendToRoom(pRoom, g_Packet, g_nPos);
    
    ROOM_LeavePlayer(pTarget, dLEAVEPLAYER_NORMAL);
    
    return 1;
}

//数据包函数的指针列表
BOOL (*MessageFuncsList[])(sPCLIENT_DATA pClient) =
{
    //PACKET_CONNECTION_CHECK               用于确认连接状态
    PACKET_ConnectionCheck,
    
    //PACKET_SERVERISFULL                   可连接数满
    PACKET_Null,
    
    //PACKET_BAD_CONNECTION                 警告用数据包
    PACKET_Null,
    
    //PACKET_LOGIN_INFO                     注册数据包
    PACKET_LoginInfo,
    
    //PACKET_LOGIN_RESULT                   注册结果
    PACKET_Null,
    
    //PACKET_GET_PLAYERINFO                 玩家信息请求
    PACKET_GetPlayerInfo,
    
    //PACKET_SEND_PLAYERINFO                玩家信息传送
    PACKET_Null,
    
    //PAKCET_NULL_PLAYER                    没有玩家数据
    PACKET_Null,
    
    //PACKET_CREATE_PLAYER                  创建新入玩家
    PACKET_CreatePlayer,
    
    //PACKET_CREATE_PLAYER_RESULT           创建新入玩家的结果
    PACKET_Null,
    
    //PACKET_WAITROOM_CLIENT_CHAT           等候室聊天
    PACKET_WaitRoomClientChat,
    
    //PACKET_WAITROOM_SERVER_CHAT           等候室聊天
    PACKET_Null,
    
    //PACKET_WHISPER_MSG                    私聊
    PACKET_WhisperMsg,
    
    //PACKET_WHISPER_SENDMSG                私聊
    PACKET_Null,
    
    //PACKET_CREATE_GAMEROOM                创建房间
    PACKET_CreateGameRoom,
    
    //PACKET_CREAT_GAMEROOM_RES             创建房间结果
    PACKET_Null,
    
    //PACKET_ENTER_GAMEROOM                 进入游戏房
    PACKET_EnterGameRoom,
    
    //PACKET_ENTER_GAMEROOM_RES             游戏房入场结果
    PACKET_Null,
    
    //PACKET_INPLAYER_INFO                  在房间里的玩家信息
    PACKET_Null,
    
    //PACKET_ROOM_OWNER_INFO                房主信息
    PACKET_Null,
    
    //PACKET_ENTER_PLAYER_INFO              入场玩家信息
    PACKET_Null,
    
    //PACKET_LEAVE_GAMEROOM                 离开房间
    PACKET_LeaveGameRoom,
    
    //PACKET_LEAVE_GAMEROOM_RES             离开房间结果
    PACKET_Null,
    
    //PACKET_LEAVE_PLAYER_INFO              离开房间的玩家的信息
    PACKET_Null,
    
    //PACKET_GAMEROOM_CLIENT_CHAT           游戏房聊天
    PACKET_GameRoomClientChat,
    
    //PACKET_GAMEROOM_SERVER_CHAT           游戏房聊天
    PACKET_Null,
    
    //PACKET_DESPORT_PLAYER                 从房间踢除某一玩家
    PACKET_DesportPlayer,
    
    //PACKET_DESPORT_DONE                   踢出玩家
    PACKET_Null,
    
    //PACKET_GET_ROOMLIST                   房间目录请求
    PACKET_GetRoomList,
    
    //PACKET_SEND_ROOMLIST                  房间目录传送
    PACKET_Null,
};

//分析通过recv函数存储到recvBuff得数据包数据,并运行对应数据包处理函数.
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
        
        if (tag >= MAX_TAG)
            return 0;
        
        //当目前m_recvSize内数据的长度小于数据包总长度时
        if (pClient->m_recvSize < msgSize) return 1;
        
        //连接到函数指针
        if (!MessageFuncsList[tag](pClient))
            return 0;
        
        memmove(&pClient->m_recvBuff[0],
                &pClient->m_recvBuff[msgSize],
                pClient->m_recvSize - msgSize);
        
        pClient->m_recvSize -= msgSize;
    }
    return 1;
}

//EOF