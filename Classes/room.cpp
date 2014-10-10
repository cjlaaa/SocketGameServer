//
//  room.cpp
//  testServer_Mac
//
//  Created by hENRYcHANG on 14-10-1.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#include "room.h"

void ROOM_DeleteData(sPROOM_DATA pData)
{
    
}

//如果在Garbage List中存在数据,则重新使用Garbage List的数据,否则malloc分配新的内存.
sPROOM_DATA ROOM_NewData()
{
    return sPROOM_DATA();
}

//房间数据结构体初始化
void ROOM_InitRoomData(sPROOM_DATA pRoom)
{
    
}

//获取空房间的编号
int ROOM_GetEmptyArray()
{
    return 0;
}

//处理离开房间的玩家
BOOL ROOM_LeavePlayer(sPCLIENT_DATA pClient,BYTE mode)
{
    return FALSE;
}

//EOF