/*************************************************************************
	> File Name: send_response.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年10月20日 星期六 16时20分34秒
 ************************************************************************/

#include<stdio.h>

//参数：长链接sockfd 响应码
int send_response(int sockfd, int id) {
    return send(sockfd, &id, 4, 0) 
}
