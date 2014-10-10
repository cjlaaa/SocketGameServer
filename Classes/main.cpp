//
//  main.cpp
//  testServer
//
//  Created by hENRYcHANG on 14-9-9.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#include <iostream>
#include "def.h"
#include "linked_list.h"
#include "structs.h"
#include "utils.h"
#include "network.h"
#include "message.h"
#include "room.h"

//全局变量
int g_ProcessLife = TRUE;

//服务器的基本套接字
SOCKET g_SOCK = INVALID_SOCKET;

sPLAYERS g_PLAYERS;
sROOMS g_ROOMS;

//总连接数
int g_TotalClient = 0;

//全局函数
void ProcessLoop();


//Win32的Main处理
#ifdef WIN32

//程序名，Class名
#define dAPP_NAME	"BOARD_Server"

HINSTANCE			hInst;		//Current instance
HWND				g_hwnd;		//Window Handle

//Windows 消息过程
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine,
					 int nCmdShow)
{
	WNDCLASS	wc;
	if (!hPrevInstance)
	{
		wc.style						=	CS_HREDRAW;
		wc.lpfnWndProc					=	(WNDPROC)WndProc;
		wc.cbClsExtra					=	0;
		wc.cbWndExtra					=	0;
		wc.hInstance					=	hInstance;
		wc.hIcon						=	NULL;
		wc.hCursor						=	LoadCursor(NULL,IDC_ARROW);
		wc.hbrBackground				=	(HBRUSH)(BLACK_BRUSH);
		wc.lpszMenuName					=	(LPSTR)IDR_MAIN_MENU;
		wc.lpszClassName				=	dAPP_NAME;
		
		if (RegisterClass(&wc)==0) return(FALSE);
	}
	hInst = hInstance;
	
	//求菜单的高度
	DWORD dwMenuHeight		=	GetSystemMetrics(SM_CYMENU);
	DWORD dwCaptionHeight	=	GetSystemMetrics(SM_CYCAPTION);
	DWORD dwFrameHeight		=	GetSystemMetrics(SM_CYSIZEFRAME);

	//输出窗口高度
	char outputString[32];
	sprintf(outputString,"Menu Height is %d\n",(dwMenuHeight + dwCaptionHeight + dwFrameHeight)*2);
	OutputDebugString(outputString);

	g_hwnd = CreateWindow(dAPP_NAME,
		dAPP_NAME,
		WS_SYSMENU|WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		10,
		350,
		(dwMenuHeight + dwCaptionHeight + dwFrameHeight)*2,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!g_hwnd) return (FALSE);

	ShowWindow(g_hwnd,nCmdShow);
	UpdateWindow(g_hwnd);

	//主循环
	ProcessLoop();
	
	return (WM_DESTROY);
}

//Windows消息过程
LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDM_SHUTDOWN:
				{
					if (MessageBox(g_hwnd,"要结束服务器的运行吗？","确认",MB_YESNO)==IDNO) break;
					SendMessage(g_hwnd,WM_DESTROY,0,0);
					g_ProcessLife = FALSE;
					break;
				}
				break;
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd,uMsg,wParam,lParam);
	}
	return 0;
}

#else

//当发生服务器程序结束的Signal时调用的函数.
void DestroySignal(int sigNum)
{
    g_ProcessLife = FALSE;
}

//为Signal句柄,初始化Signal的函数
void InitSignal()
{
    //防止因SIGPIPE signal而引起的强制结束服务器运行的问题而编写的程序代码.
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    act.sa_flags &= -SA_RESETHAND;
    sigaction(SIGPIPE, &act, NULL);
    
    //当发生强制结束服务器运行的Signal时,调用正常结束服务器运行函数的Signal的设置
    signal(SIGINT, DestroySignal);
    signal(SIGKILL, DestroySignal);
    signal(SIGQUIT, DestroySignal);
    signal(SIGTERM, DestroySignal);
}

int main()
{
    InitSignal();
    ProcessLoop();
    return TRUE;
}

#endif

//服务器基本数据初始化函数
BOOL InitServerData()
{
    int i;
    
    //INIT g_PLAYERS
    g_PLAYERS.m_ClientList                  = NULL;
    g_PLAYERS.m_ClientMemoryList            = NULL;
    g_PLAYERS.m_InWaitRoomList              = NULL;
    
    g_PLAYERS.m_totalMemoryCnt              = 0;
    g_PLAYERS.m_totalPlayerCnt              = 0;
    
    for (i = 0; i < dSTR_HASHKEY_MAX; i++)
    {
        g_PLAYERS.m_IdList[i] = NULL;
        g_PLAYERS.m_NameList[i] = NULL;
    }
    
    //INIT g_ROOMS
    g_ROOMS.m_RoomList                      = NULL;
    g_ROOMS.m_GameRoomList                  = NULL;
    g_ROOMS.m_RoomMemoryList                = NULL;
    
    g_ROOMS.m_RoomCnt                       = 0;
    g_ROOMS.m_GameRoomCnt                   = 0;
    g_ROOMS.m_RoomMemoryCnt                 = 0;
    
    for (i = 0; i < dMAX_ROOM_CNT; i++)
        g_ROOMS.m_roomArray[i] = NULL;
    
    return 1;
}

//服务器基本数据删除函数
void DestroyServerData()
{
    //断开与玩家的链接
    sPCLIENT_DATA client,next_client;
    LIST_WHILE(g_PLAYERS.m_ClientList, client, next_client, m_next);
    DisconnectClient(client);
    LIST_WHILEEND(g_PLAYERS.m_ClientList, client, next_client);
    
    //释放客户的Garbage
    LIST_WHILE(g_PLAYERS.m_ClientMemoryList, client, next_client, m_next);
    free(client);
    client = NULL;
    LIST_WHILEEND(g_PLAYERS.m_ClientMemoryList, client, next_client);
    
    sPROOM_DATA room,next_room;
    
    LIST_WHILE(g_ROOMS.m_RoomList, room, next_room, m_next);
    REMOVE_FROM_LIST(g_ROOMS.m_RoomList, room, m_prev, m_next);
    ROOM_DeleteData(room);
    LIST_WHILEEND(g_ROOMS.m_RoomList, room, next_room);
    
    //释放房间数据的Garbage
    LIST_WHILE(g_ROOMS.m_RoomMemoryList, room, next_room, m_next);
    free(room);
    room = NULL;
    LIST_WHILEEND(g_ROOMS.m_RoomMemoryList, room, next_room);
}

//Win32,FreeDSB和Linux共同的主循环
void ProcessLoop()
{
#ifdef WIN32
    //当是Win32时,利用WSAStartup初始化Winsock DLL.
    WSADATA WSAData;
	WSAStartup(0x0101,&WSAData);

	//用于Windows消息句柄的Msg变量
	MSG msg;
#endif
    
    //服务器套接字初始化
    g_SOCK = InitServerSock(dSERVER_PORT, dMAX_LISTEN);
    if (g_SOCK < 0)
    {
        log("InitServerSock Failed!\r\n");
        return;
    }
    
    //服务器数据初始化
    if(!InitServerData())
    {
        log("InitServerData Failed!\r\n");
        return;
    }
    else
    {
        log("Chat Server Started.\r\n");
    }
    
    fd_set read_set;
    fd_set write_set;
    fd_set exc_set;
    struct timeval tv;
    SOCKET nfds;
    
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    sPCLIENT_DATA client, next_client;
    
    while (g_ProcessLife)
    {
#ifdef WIN32
        //Win32消息句柄
		if (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if (!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
#endif
		//fd_set初始化
        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
        FD_ZERO(&exc_set);
        
        FD_SET(g_SOCK,&read_set);
        nfds = g_SOCK;

        //求套接字的最大值
        LIST_WHILE(g_PLAYERS.m_ClientList, client, next_client, m_next);
        
        if (client->m_sock > nfds) nfds = client->m_sock;
        
        FD_SET(client->m_sock, &read_set);
        FD_SET(client->m_sock, &write_set);
        FD_SET(client->m_sock, &exc_set);
        
        LIST_WHILEEND(g_PLAYERS.m_ClientList, client, next_client);
        
        //select
        if (select(nfds + 1, &read_set, &write_set, &exc_set, &tv) < -1)
        {
            log("select Error!\r\n");
            continue;
        }
        
        //新的连接请求的处理
        if (FD_ISSET(g_SOCK, &read_set)) AcceptNewClient(g_SOCK);
        
        //例外错误处理和数据接收
        LIST_WHILE(g_PLAYERS.m_ClientList, client, next_client, m_next);
        
        //错误!
        if (FD_ISSET(client->m_sock, &exc_set))
        {
            DisconnectClient(client);
            LIST_SKIP(client, next_client);
        }
        
        //超过1分钟没反应时断开连接
        if (client->m_lastRecvTime + 60000 <= timeGetTime())
        {
            DisconnectClient(client);
            LIST_SKIP(client, next_client);
        }
        
        //当存在可recv的数据时
        if (FD_ISSET( client->m_sock, &read_set))
        {
            if (!RecvFromClient(client))
            {
                DisconnectClient(client);
                LIST_SKIP(client, next_client);
            }
        }
        
        //当存在已经recv的数据时
        if (client->m_recvSize)
        {
            if (!ReadRecvBuff(client))
            {
                DisconnectClient(client);
                LIST_SKIP(client, next_client);
            }
        }
        LIST_WHILEEND(g_PLAYERS.m_ClientList,client,next_client);
        
        //Send Buff Flush
        LIST_WHILE(g_PLAYERS.m_ClientList, client, next_client, m_next);
        if (client->m_sendSize && FD_ISSET(client->m_sock, &write_set))
        {
            if (FlushSendBuff(client) < 0)
            {
                DisconnectClient(client);
                LIST_SKIP(client, next_client);
            }
        }
        LIST_WHILEEND(g_PLAYERS.m_ClientList, client, next_client);
    }
    
    //服务器数据的删除
    DestroyServerData();
    
    //服务器套接字的关闭
    closesocket(g_SOCK);
    
    //释放Winsock DLL
#ifdef WIN32
    WSACleanup();
#endif
}

//EOF
