//
//  def.h
//  testServer
//  多平台头文件定义
//  
//  Created by hENRYcHANG on 14-9-9.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#ifndef testServer_def_h
#define testServer_def_h

#define dSERVER_PORT    7000
#define dMAX_LISTEN     200

//当使用select时,为了重新设置FD_SETSIZE限制,而进行的Define.
//当Linux或FreeBSD时,默认值是1024;当Win32时,默认值是64.
#define FD_SETSIZE      6000

//Win32的头文件
#if defined(_WIN32)
#include <windows.h>
#include <winsock.h>
#include <mmsystem.h>
#include <stdio.h>
#include <time.h>
#include "../testServer_Win32/testServer_Win32/resource.h"

//以下两句不写会出现"无法解析的外部符号"错误.
//链接到WS2_32.lib(也可以使用wsock32.lib),提供winsock相关函数实现.
#pragma comment(lib,"WS2_32")
//链接到winmm.lib,提供timeGetTime函数实现.
#pragma comment(lib,"winmm")
//提供WinMain相关函数实现
#pragma comment(lib,"user32")


#else
//FreeBSD,Linux的头文件
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

//为Win32中的开发和便利的define,typedef
//在UNIX系列操作系统中利用close函数关闭套接字
#define closesocket(sock) close(sock)
#define INVALID_SOCKET (-1)
#define TRUE 1
#define FALSE 0
typedef int             SOCKET;
typedef int             BOOL;
typedef unsigned char   BYTE;
typedef unsigned long   DWORD;
typedef unsigned short  WORD;

#endif  //end of WIN32


#endif

//EOF