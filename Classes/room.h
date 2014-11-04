//
//  room.h
//  testServer_Mac
//  房间处理
//
//  Created by hENRYcHANG on 14-10-1.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#ifndef __testServer_Mac__room__
#define __testServer_Mac__room__

#include "structs.h"

/////////////////////////////////
// D E F I N E S
/////////////////////////////////
#define dROOM_TYPE_NORMAL       0   //一般房间
#define dROOM_TYPE_PRIVATE      1   //私密房间

#define dROOM_STATE_WAIT        0   //等待中
#define dROOM_STATE_LOADING     1   //注册中
#define dROOM_STATE_GAME        2   //游戏中

#define dLEAVEPLAYER_NORMAL     0   //一般
#define dLEAVEPLAYER_CONCLOSE   1   //连接被断开的情况

/////////////////////////////////
// G L O B A L  F U N C
/////////////////////////////////
sPROOM_DATA ROOM_NewData();
void        ROOM_DeleteData(sPROOM_DATA pData);
void        ROOM_InitRoomData(sPROOM_DATA pRoom);

int ROOM_GetEmptyArray();

BOOL ROOM_LeavePlayer(sPCLIENT_DATA pClient,BYTE mode);

#endif /* defined(__testServer_Mac__room__) */

//EOF
