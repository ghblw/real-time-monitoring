/*************************************************************************
	> File Name: recv_response.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年10月20日 星期六 16时28分52秒
 ************************************************************************/

#include<stdio.h>

//参数：长链接sockfd
int recv_response(int sockfd) {
    int id;
    return recv(sockfd, &id, 4, 0);
}
