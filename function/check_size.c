/*************************************************************************
	> File Name: check_size.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年10月20日 星期六 15时15分58秒
 ************************************************************************/

#include<stdio.h>

//参数：数据文件路径 指定文件大小限制 压缩文件备份目录
int check_size(char *filename, int size, char *dir) {
    struct stat data;
    stat(filename, &data);
    int size_m = data.st_size / 1048576;
    if (size_m <= size) return 0; 
    FILE *fd = popen(filename, "r");
    char cmd1[100], cmd2[100], cmd3[100], backup_name[100];
    
    time_t time_log = time(NULL);
    struct tm* tm_log = localtime(&time_log);
    sprintf(time, "%04d_%02d_%02d_%02d:%02d:%02d", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);//获取时间
    sprintf(backup_name,"%s_%s",filename, time);
    sprintf(cmd1, "cp %s %s/%s", filename, dir, backup_name); //按时间命名拷贝
    system(cmd1);
    sprintf(cmd2, "tar -cvzf %s/%s.tar.gz %s", dir, backup_name, filename);// 压缩文件到备份目录
    sprintf(cmd3, "rm %s/%s")//删除拷贝文件
    system(cmd3);
    return 1;
}
