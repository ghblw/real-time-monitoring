/*************************************************************************
	> File Name: readrc.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年09月30日 星期日 10时22分15秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include "head.h"
#define MAXN 100
//参数：文件路径 提取变量名 需要存到的变量
//读取配置文件,'='两端不带空格 
int get_conf_value (char *pathname, char *key_name, char *value) {
    int flag = 0;
    int str_len = strlen(key_name);//匹配字符串长度
    FILE *fp = fopen(pathname, "r");
    char *line = NULL;//读取每一行
    ssize_t read;
    size_t len = 0;//read为每行读取字符数量,len = 0 系统自动分配内存
    while ((read = getline(&line, &len, fp)) != -1) {
        char *p = strstr(line, key_name);
        if(p != NULL) {
            if(line[str_len] == '=') {
                strncpy(value, &line [str_len + 1], read -str_len - 2);//将变量的值拷贝到value
                int len2 = strlen(value);
                value[len2] = '\0';
                flag = 1;
                return 1;
            }
        }
    }
    if (flag == 0) {
        printf("No such variable");
        return -1;
    }
}

int main (int argc,char *argv[]) {
    char *value = (char *)malloc(sizeof(char) * MAXN);    //需要分配空间
    get_conf_value(argv[1], argv[2], value) && printf("%s\n", value);
}
