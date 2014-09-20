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

#define dPACKET_ALIVE_CHECK         0//用于连接状态的确认
#define dPACKET_CHANGE_NICK         1//绰号的修改
#define dPACKET_RECV_MSG            2//聊天消息(客户->服务器)
#define dPACKET_SEND_MSG            3//聊天消息(服务器->客户)

BOOL ReadRecvBuff(sPCLIENT_DATA pClient);

#endif /* defined(__testServer_Mac__message__) */

//EOF