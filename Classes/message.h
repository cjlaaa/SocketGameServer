//
//  message.h
//  testServer_Mac
//
//  Created by hENRYcHANG on 14-9-10.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#ifndef __testServer_Mac__message__
#define __testServer_Mac__message__

#include "structs.h"

#define dCHECK_SPEEDHACK_TIME       10000

enum
{
    //连接状态确认用
    PACKET_CONNECTION_CHECK = 0,
    
    //可连接数满
    PACKET_SERVERISFULL,
    
    //警告用数据包
    PACKET_BAD_CONNECTION,
    
    //注册数据包
    PACKET_LOGIN_INFO,
    
    //注册结果
    PACKET_LOGIN_RESULT,
    
    //玩家信息请求
    PACKET_GET_PLAYERINFO,
    
    //玩家信息传送
    PACKET_SEND_PLAYERINFO,
    
    //没有玩家数据
    PACKET_NULL_PLAYER,
    
    //创建新入玩家
    PACKET_CREATE_PLAYER,
    
    //创建新入玩家的结果
    PACKET_CREATE_PLAYER_RESULT,
    
    //等候室聊天
    PACKET_WAITROOM_CLIENT_CHAT,
    
    //等候室聊天
    PACKET_WAITROOM_SERVER_CHAT,
    
    //私聊
    PACKET_WHISPER_MSG,
    
    //私聊
    PACKET_WHISPER_SENDMSG,
    
    //创建房间
    PACKET_CREATE_GAMEROOM,
    
    //创建房间结果
    PACKET_CREAT_GAMEROOM_RES,
    
    //进入游戏房
    PACKET_ENTER_GAMEROOM,
    
    //游戏房入场结果
    PACKET_ENTER_GAMEROOM_RES,
    
    //在房间里的玩家信息
    PACKET_INPLAYER_INFO,
    
    //房主信息
    PACKET_ROOM_OWNER_INFO,
    
    //入场玩家信息
    PACKET_ENTER_PLAYER_INFO,
    
    //离开房间
    PACKET_LEAVE_GAMEROOM,
    
    //离开房间结果
    PACKET_LEAVE_GAMEROOM_RES,
    
    //离开房间的玩家的信息
    PACKET_LEAVE_PLAYER_INFO,
    
    //游戏房聊天
    PACKET_GAMEROOM_CLIENT_CHAT,
    
    //游戏房聊天
    PACKET_GAMEROOM_SERVER_CHAT,
    
    //从房间踢除某一玩家
    PACKET_DESPORT_PLAYER,
    
    //踢出玩家
    PACKET_DESPORT_DONE,
    
    //房间目录请求
    PACKET_GET_ROOMLIST,
    
    //房间目录传送
    PACKET_SEND_ROOMLIST,
    
    MAX_TAG
};

/////////////////////////////////
// D E F I N E S
/////////////////////////////////
#define dLOGIN_RES_SUCCESS              0   //注册成功
#define dLOGIN_RES_BADPW                1   //口令错
#define dLOGIN_RES_USEDID               2   //已经建立连接的ID
#define dLOGIN_RES_EXCEPTION            3   //例外错误

#define dCREATE_PLAYER_RES_SUCCESS      0   //创建成功
#define dCREATE_PLAYER_RES_USEDNAME     1   //使用中的名称
#define dCREATE_PLAYER_RES_EXCEPTION    2   //例外错误

#define dCREATE_ROOM_RES_SUCCESS        0   //创建成功
#define dCREATE_ROOM_RES_FULL           1   //没有空房间
#define dCREATE_ROOM_RES_EXCEPTION      2   //例外错误

#define dENTER_GAMEROOM_RES_SUCCESS     0   //入场成功
#define dENTER_GAMEROOM_RES_CANTFIND    1   //没有指定的房间
#define dENTER_GAMEROOM_RES_FULL        2   //房间满
#define dENTER_GAMEROOM_RES_BADPASS     3   //口令错
#define dENTER_GAMEROOM_RES_EXCEPTION   4   //例外错误

#define dLEAVE_GAMEROOM_RES_SUCCESS     0   //退场成功
#define dLEAVE_GAMEROOM_RES_EXCEPTION   1   //例外错误

/////////////////////////////////
// G L O B A L  F U N C
/////////////////////////////////
//分析通过recv函数存储到recvBuff的数据包数据,并执行对应数据包函数
BOOL ReadRecvBuff(sPCLIENT_DATA pClient);

#endif /* defined(__testServer_Mac__message__) */

//EOF