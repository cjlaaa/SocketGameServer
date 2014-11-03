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

//当从服务器向客户端发送的数据包回到服务器时,需要使用函数指针数据,为此,为了处理例外情况,需要如下所示的函数
BOOL PACKET_Null(sPCLIENT_DATA pClient)
{
    return TRUE;
}

//为了防止通过timeGetTime等时间函数的Hooking,加速时间的Speedhack类攻击工具的使用,而进行的数据包的处理.
BOOL PACKET_ConnectionCheck(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//注册处理
BOOL PACKET_LoginInfo(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//客户端的玩家数据传送请求
BOOL PACKET_GetPlayerInfo(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//新入玩家数据的创建
BOOL PACKET_CreatePlayer(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//等候室中的聊天
BOOL PACKET_WaitRoomClientChat(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//私聊处理
BOOL PACKET_WhisperMsg(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//创建新的房间
BOOL PACKET_CreateGameRoom(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//传送创建的房间列表
BOOL PACKET_GetRoomList(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//要进入所创建房间的客户请求
BOOL PACKET_EnterGameRoom(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//离开房间
BOOL PACKET_LeaveGameRoom(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//在房间内的聊天
BOOL PACKET_GameRoomClientChat(sPCLIENT_DATA pClient)
{
    return FALSE;
}

//剔除玩家
BOOL PACKET_DesportPlayer(sPCLIENT_DATA pClient)
{
    return FALSE;
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