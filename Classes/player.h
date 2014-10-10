//
//  player.h
//  testServer_Mac
//  玩家
//
//  Created by hENRYcHANG on 14-10-1.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#ifndef __testServer_Mac__player__
#define __testServer_Mac__player__

#include "structs.h"

/////////////////////////////////
// D E F I N E S
/////////////////////////////////
enum
{
    PLAYER_STATE_NONE = 0,
    PLAYER_STATE_LOGIN,         //注册
    PLAYER_STATE_DATAREADY,     //玩家数据传送等待
    PLAYER_STATE_CREATEPLAYER,  //新玩家的生成
    PLAYER_STATE_WAITROOM,      //等候室
    PLAYER_STATE_INROOM,        //游戏房(等待中)
    PLAYER_STATE_INGAME,        //游戏房(游戏中)
};

#define GET_PLAYER_ID(p) (p->m_PLAYER.m_id)
#define GET_PLAYER_STATE(p) (p->m_State)

/////////////////////////////////
// G L O B A L  F U N C
/////////////////////////////////
sPCLIENT_DATA PLAYER_NewData();
void PLAYER_DeleteData(sPCLIENT_DATA pData);

void PLAYER_InitPlayerData(sPCLIENT_DATA pClient,SOCKET sock,char *ip);
BOOL PLAYER_CheckValidPassword(sPCLIENT_DATA pClient,char *pw);

BOOL PLAYER_LoadPlayerData(sPCLIENT_DATA pClient);
void PLAYER_SavePlayerData(sPCLIENT_DATA pClient);

BOOL PLAYER_CheckUsedName(char *name);
void PLAYER_CreateNewPlayerData(sPCLIENT_DATA pClient);

BOOL PLAYER_CheckConnectedID(char *id);

sPCLIENT_DATA PLAYER_FindPlayerByID(char *id);
sPCLIENT_DATA PLAYER_FindPlayerByName(char *name);

sPCLIENT_DATA PLAYER_FindPlayerInRoom(sPROOM_DATA pRoom,char *name);

#endif /* defined(__testServer_Mac__player__) */

//EOF
