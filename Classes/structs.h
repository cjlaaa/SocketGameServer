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

//身份证(id),玩家名hash用
#define dSTR_HASHKEY_ETC    128
#define dSTR_HASHKEY_MAX    (dSTR_HASHKEY_ETC+1)

//注册ID,玩家角色名的最大长度
#define dID_LEN             50
#define dNAME_LEN           50

//房间名长度
#define dROOM_NAME_LEN      128

//游戏房口令长度
#define dROOM_PASSWORD_LEN  20

//最大游戏房数
#define dMAX_ROOM_CNT       100

//hostname长度
#define dIP_LEN             20

//send,recv用缓冲器大小
#define dMAX_RECV_BUFF      10240
#define dMAX_SEND_BUFF      (dMAX_RECV_BUFF*10)

//游戏房数据结构体
struct room_data
{
    //房间号
    int                     m_roomNum;
    //游戏房标题
    char                    m_roomName[dROOM_NAME_LEN];
    //游戏房类型
    BYTE                    m_roomType;
    //私密游戏房口令
    char                    m_password[dROOM_PASSWORD_LEN];
    //游戏房状态
    BYTE                    m_roomState;
    
    //游戏房内用户总数
    BYTE                    m_inPlayerCnt;
    
    //允许的最大用户数
    BYTE                    m_MaxPlayer;
    
    //游戏房内的玩家列表
    struct client_data      *m_inPlayer;
    //房主的客户数据指针
    struct client_data      *m_roomOwner;
    
    //游戏房列表用prev,next
    struct room_data        *m_prev;
    struct room_data        *m_next;
    
    //玩家房列表用prev,next
    struct room_data        *m_gamePrev;
    struct room_data        *m_gameNext;
};
typedef struct room_data sROOM_DATA,*sPROOM_DATA;

//玩家数据结构体
struct player_data
{
    //注册ID,角色名
    char                    m_id[dID_LEN];
    char                    m_name[dNAME_LEN];
    
    //等级
    int                     m_grade;
    int                     m_money;
    
    //TODO:其他必要数据的追加
};
typedef  struct player_data sPLAYER_DATA,*sPPLAYER_DATA;

//客户数据结构体
struct client_data
{
    //用于数据传送的套接字等各种数据变量
    SOCKET          m_sock;
    
    char            m_Ip[dIP_LEN];
    
    char            m_recvBuff[dMAX_RECV_BUFF];
    int             m_recvSize;
    int             m_recvPos;
    
    char            m_sendBuff[dMAX_SEND_BUFF];
    int             m_sendSize;
    
    //用于确认连接状态的最后recv时间值
    DWORD           m_lastRecvTime;
    //用于防止提速攻击(Speed Hack)
    DWORD           m_lastCheckTime;
    BYTE            m_isBadConnection;
    
    //玩家数据结构体
    sPLAYER_DATA    m_Player;
    
    //游戏房数据结构体指针
    sPROOM_DATA     m_pRoom;
    
    //客户状态值
    BYTE            m_State;
    
    //用于链表连接的指针
    struct client_data *m_prev;
    struct client_data *m_next;
    
    struct client_data *m_game_prev;
    struct client_data *m_game_next;
    
    struct client_data *m_wait_prev;
    struct client_data *m_wait_next;
    
    struct client_data *m_name_prev;
    struct client_data *m_name_next;
    
    struct client_data *m_id_prev;
    struct client_data *m_id_next;
};
typedef struct client_data sCLIENT_DATA,*sPCLIENT_DATA;

typedef struct
{
    //客户数据列表
    sPCLIENT_DATA m_ClientList;
    //内存数据列表
    sPCLIENT_DATA m_ClientMemoryList;
    sPCLIENT_DATA m_InWaitRoomList;
    
    sPCLIENT_DATA m_NameList[dSTR_HASHKEY_MAX];
    sPCLIENT_DATA m_IdList[dSTR_HASHKEY_MAX];
    
    //连接者总数(m_ClientList列表数据数)
    int m_totalPlayerCnt;
    //内存容量总数(m_ClientMemoryList列表数)
    int m_totalMemoryCnt;
} sPLAYERS;

typedef struct
{
    //整个游戏房列表,
    //正在玩游戏中的游戏房列表
    //游戏房内存列表(Memory Garbage List)
    sPROOM_DATA     m_RoomList;
    sPROOM_DATA     m_GameRoomList;
    sPROOM_DATA     m_RoomMemoryList;
    
    //游戏房总数
    //正在玩游戏的游戏房总数
    //内存数据总数
    int             m_RoomCnt;
    int             m_GameRoomCnt;
    int             m_RoomMemoryCnt;
    
    //游戏房数据指针数组
    sPROOM_DATA     m_roomArray[dMAX_ROOM_CNT];
} sROOMS,*sPROOMS;

///////////////////////////////////////
// G L O B A L
///////////////////////////////////////
extern SOCKET       g_SOCK;

extern sPLAYERS     g_PLAYERS;
extern sROOMS      g_ROOMS;

#endif

//EOF