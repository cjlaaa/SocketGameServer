//
//  utils.h
//  testServer_Mac
//  各种函数
//
//  Created by hENRYcHANG on 14-9-10.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#ifndef testServer_Mac_utils_h
#define testServer_Mac_utils_h

#include "def.h"

//日志的画面输出和文件记录函数
void log(char *buf,...);

//Win32 timeGetTime函数的仿效(emulation).
#ifndef WIN32
DWORD timeGetTime();
#endif

int GetStrHashIndex(char *str);

#endif

//EOF