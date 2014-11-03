//
//  room.cpp
//  testServer_Mac
//
//  Created by hENRYcHANG on 14-10-1.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#include "room.h"
#include "linked_list.h"
#include "utils.h"
#include "packet.h"
#include "message.h"
#include "network.h"
#include "player.h"

extern char g_Packet[dMAX_SEND_BUFF];
extern int g_nPos;

void ROOM_DeleteData(sPROOM_DATA pData)
{
    /*
     好吧,我来描述一下这个坑,昨天几乎坑我一下午.
     首先,刚开始写到INSERT_TO_LIST这里,就一直出现"Expected expression","expected expression before ';'"等问题.无论是在XCode还是Eclipse上都有这个问题.XCode指示问题处在m_next与回括号之间.反复检查,代码都没有任何问题,将宏展开写也没有任何问题.
     后来,在vs中打开工程,将所有文件从UTF-8无签名改成UTF-8带签名(例行处理),可以正常编译,于是就将代码按平台处理,在Win32下使用宏,非Win32将宏展开写.
     最后将在vs下处理过的代码拿回xcode打开,终于发现问题:在m_next与回括号之间有一个奇怪的字符(类似倒问号),将此字符删除,问题迎刃而解.
     看来是由于文档编码格式问题造成的.总算找到这个坑逼问题的原因了.
    
    */
    /*问题开始*/
    //在xcode和eclipse上使用INSERT_TO_LIST链表宏会报奇怪的问题.用visual studio却能编译通过.
    //所以在非Win32状态下,将宏展开处理.
//#ifdef WIN32
    INSERT_TO_LIST(g_ROOMS.m_RoomMemoryList, pData, m_prev, m_next);
//#else
//    if(!g_ROOMS.m_RoomMemoryList)
//    {
//        g_ROOMS.m_RoomMemoryList = pData;
//        g_ROOMS.m_RoomMemoryList->m_prev = g_ROOMS.m_RoomMemoryList->m_next = g_ROOMS.m_RoomMemoryList;
//    }
//    else
//    {
//        pData->m_prev = g_ROOMS.m_RoomMemoryList->m_prev;
//        pData->m_next = g_ROOMS.m_RoomMemoryList;
//        g_ROOMS.m_RoomMemoryList->m_prev = pData;
//        pData->m_prev->m_next = pData;
//    }
//#endif
    /*问题结束*/
    
    g_ROOMS.m_RoomMemoryCnt++;
}

//如果在Garbage List中存在数据,则重新使用Garbage List的数据,否则malloc分配新的内存.
sPROOM_DATA ROOM_NewData()
{
    sPROOM_DATA pData = NULL;
    
    //若在Garbage List中有数据
    if (g_ROOMS.m_RoomMemoryList)
    {
        pData = g_ROOMS.m_RoomMemoryList;
        
        REMOVE_FROM_LIST(g_ROOMS.m_RoomMemoryList, pData, m_prev, m_next);
        g_ROOMS.m_RoomMemoryCnt--;
        return pData;
    }
    else
    {
        pData = (sPROOM_DATA)malloc(sizeof(sROOM_DATA));
        
        if (!pData)
            return NULL;
        
        return pData;
    }
    
    return pData;
}

//房间数据结构体初始化
void ROOM_InitRoomData(sPROOM_DATA pRoom)
{
    pRoom->m_roomNum = -1;
    *pRoom->m_roomName = '\0';
    pRoom->m_roomType = dROOM_TYPE_NORMAL;
    *pRoom->m_password = '\0';
    pRoom->m_roomState = dROOM_STATE_WAIT;
    pRoom->m_inPlayerCnt = 0;
    pRoom->m_inPlayer = NULL;
    pRoom->m_roomOwner = NULL;
    pRoom->m_prev = NULL;
    pRoom->m_next = NULL;
    pRoom->m_gamePrev = NULL;
    pRoom->m_gameNext = NULL;
}

//获取空房间的编号
int ROOM_GetEmptyArray()
{
    for (int i = 0; i < dMAX_ROOM_CNT; i++)
    {
        if (!g_ROOMS.m_roomArray[i])
            return i;
    }
    return -1;
}

//处理离开房间的玩家
BOOL ROOM_LeavePlayer(sPCLIENT_DATA pClient,BYTE mode)
{
    sPROOM_DATA pRoom = pClient->m_pRoom;
    
    //例外处理
    if (!pRoom)
    {
        log("Room_LeavePlayer : !pRoom\r\n");
        return 0;
    }
    
    //留在房间的人只有一个人时
    if (pRoom->m_inPlayerCnt<=1)
    {
        if (mode==dLEAVEPLAYER_NORMAL)
        {
            g_nPos = 2;
            
            PutWord(g_Packet,PACKET_LEAVE_GAMEROOM_RES,g_nPos);
            PutByte(g_Packet, dLEAVE_GAMEROOM_RES_SUCCESS, g_nPos);
            
            PutSize(g_Packet, g_nPos);
            
            SendData(pClient,g_Packet,g_nPos);
        }
        
        REMOVE_FROM_LIST(pRoom->m_inPlayer, pClient, m_game_prev, m_game_next);
        pRoom->m_inPlayerCnt--;
        
        pClient->m_pRoom = NULL;
        
        INSERT_TO_LIST(g_PLAYERS.m_InWaitRoomList,pClient, m_wait_prev, m_wait_next);
        
        GET_PLAYER_STATE(pClient) = PLAYER_STATE_WAITROOM;
        
        g_ROOMS.m_roomArray[pRoom->m_roomNum] = NULL;
        REMOVE_FROM_LIST(g_ROOMS.m_RoomList, pRoom, m_prev, m_next);
        g_ROOMS.m_RoomCnt++;
        
        ROOM_DeleteData(pRoom);
        
        return 1;
    }
    else
    {
        //离开房间的是玩家是版主时
        if (pRoom->m_roomOwner==pClient)
        {
            if (mode==dLEAVEPLAYER_NORMAL)
            {
                g_nPos = 2;
                
                PutWord(g_Packet, PACKET_LEAVE_GAMEROOM_RES, g_nPos);
                PutByte(g_Packet, dLEAVE_GAMEROOM_RES_SUCCESS, g_nPos);
                
                PutSize(g_Packet, g_nPos);
                
                SendData(pClient, g_Packet, g_nPos);
            }
            
            REMOVE_FROM_LIST(pRoom->m_inPlayer, pClient, m_game_prev, m_game_next);
            pRoom->m_inPlayerCnt--;
            pClient->m_pRoom = NULL;
            
            INSERT_TO_LIST(g_PLAYERS.m_InWaitRoomList, pClient, m_wait_prev, m_wait_next);
            
            GET_PLAYER_STATE(pClient) = PLAYER_STATE_WAITROOM;
            pRoom->m_roomOwner = pRoom->m_inPlayer;
            
            g_nPos = 2;
            
            PutWord(g_Packet, PACKET_LEAVE_PLAYER_INFO, g_nPos);
            PutString(g_Packet, pClient->m_Player.m_name, g_nPos);
            
            PutSize(g_Packet, g_nPos);
            
            SendToRoom(pRoom, g_Packet, g_nPos);
            
            //传送新版主的信息
            g_nPos = 2;
            
            PutWord(g_Packet, PACKET_ROOM_OWNER_INFO, g_nPos);
            PutString(g_Packet, pRoom->m_roomOwner->m_Player.m_name, g_nPos);
            
            PutSize(g_Packet, g_nPos);
            
            SendToRoom(pRoom, g_Packet, g_nPos);
            
            return 1;
        }
        else
        {
            if (mode==dLEAVEPLAYER_NORMAL)
            {
                g_nPos = 2;
                
                PutWord(g_Packet, PACKET_LEAVE_GAMEROOM_RES, g_nPos);
                PutByte(g_Packet, dLEAVE_GAMEROOM_RES_SUCCESS, g_nPos);
                
                PutSize(g_Packet, g_nPos);
                
                SendData(pClient, g_Packet, g_nPos);
            }
            
            REMOVE_FROM_LIST(pRoom->m_inPlayer, pClient, m_game_prev, m_game_next);
            
            pRoom->m_inPlayerCnt--;
            pClient->m_pRoom = NULL;
            
            INSERT_TO_LIST(g_PLAYERS.m_InWaitRoomList, pClient, m_wait_prev, m_wait_next);
            
            GET_PLAYER_STATE(pClient) = PLAYER_STATE_WAITROOM;
            
            g_nPos = 2;
            
            PutWord(g_Packet, PACKET_LEAVE_PLAYER_INFO, g_nPos);
            PutString(g_Packet, pClient->m_Player.m_name, g_nPos);
            
            PutSize(g_Packet, g_nPos);
            
            SendToRoom(pRoom, g_Packet, g_nPos);
            
            return 1;
        }
    }
    
    return 0;
}

//EOF