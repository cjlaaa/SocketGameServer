//
//  utils.cpp
//  testServer_Mac
//  各种有用函数
//
//  Created by hENRYcHANG on 14-9-10.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#include <stdarg.h>
#include "utils.h"
#include "structs.h"

//利用va_list,使日志的画面输出和记录函数,使用sprintf等格式化方法.
void log(char *buf,...)
{
    char tmp_buf[1024],tmp_buf2[1024];
    va_list args;
    FILE *fp;
    
    time_t tval;
    time(&tval);
    struct tm *currTM = localtime(&tval);
    
    fp = fopen("log.txt","a");
    
    if (!fp) return;
    
    va_start(args,buf);
    vsprintf(tmp_buf,buf,args);
    va_end(args);
    
    sprintf(tmp_buf2,"[%d/%02d/%02d %02d:%02d:%02d]%s",
            currTM->tm_year+1900,
            currTM->tm_mon+1,
            currTM->tm_mday,
            currTM->tm_hour,
            currTM->tm_min,
            currTM->tm_sec,
            tmp_buf);
    
    strcpy(tmp_buf,tmp_buf2);
    
    fprintf(fp,tmp_buf);
    
#ifndef WIN32
    printf(tmp_buf);
#endif
    
    fclose(fp);
}

//在Linux,FreeBSD中,没有以ms(毫秒)为单位获取cpu时间的Win32的timeGetTime函数
#ifndef WIN32
DWORD timeGetTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    DWORD value = (((tv.tv_sec - 1000000000)*1000)+(tv.tv_usec/1000));
    return value;
}
#endif

//求得hash index
int GetStrHashIndex(char *str)
{
    int hashIdx;
    
    hashIdx = abs( str[0] / 2 );
    
    if ( hashIdx < 0 || hashIdx > dSTR_HASHKEY_ETC )
        hashIdx = dSTR_HASHKEY_ETC;
    
    return hashIdx;
}

//EOF