/*************************************************************************
	> File Name: client0.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年12月16日 星期日 14时07分38秒
 ************************************************************************/

#include<stdio.h>
#include "../common/common.c"
int main(int argc, char *argv[]) {
    int sockfd = socket_connect(1080, argv[1]);
}
