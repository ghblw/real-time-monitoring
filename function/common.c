/*************************************************************************
	> File Name: common.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年09月29日 星期六 14时52分16秒
 ************************************************************************/

#include<stdio.h>
#include "head.h"
#define MAXN 10000
//服务端创建套接字
int socket_create(char *port1) { 
    int sock_server,port;
    port = atoi(port1);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((sock_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        return -1;
    }
    if (bind(sock_server, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) < 0) {
        perror("Bind");
        return -1;
    }
    if (listen(sock_server, MAXN) < 0) {
        perror("Listen");
        return -1;
    }
    printf("Listening\n");
    return sock_server;
}
