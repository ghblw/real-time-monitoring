/*************************************************************************
	> File Name: pi_log.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年09月30日 星期日 15时57分28秒
 ************************************************************************/

#include<stdio.h>
#include "head.h"

//参数：文件路径 格式控制字符串 参数列表
//返回值：输出字符数 
int write_Pi_log (char *PiHealthLog, const char *format, ...) {
    va_list arg;
    printf("!");
    FILE *fp = fopen(PiHealthLog, "a+");
    printf("!");
    int ret = 0;
    va_start(arg, format);
    
    time_t time_log = time(NULL);
    struct tm* tm_log = localtime(&time_log);
    fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d ",tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);
    vfprintf(fp, format, arg);
    va_end(arg);
    fclose(fp);
    return ret;
}
int main (int argc, char *argv[]) {
    write_Pi_log("/home/yuxiaowei/log", "Connect %s Success\n",argv[1]);
}
