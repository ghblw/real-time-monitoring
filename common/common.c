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
int socket_create(int port) { 
    int sock_server;
    //port = atoi(port1);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((sock_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        return -1;
    }
    //设置套接字选项避免地址使用错误  
    int on = 1;  
    if((setsockopt(sock_server, SOL_SOCKET,SO_REUSEADDR, &on, sizeof(on))) < 0) {
        perror("setsockopt failed");
    } 
    if (bind(sock_server, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) < 0) {
        perror("Bind");
        return -1;
    }
    if (listen(sock_server, 20) < 0) {
        perror("Listen");
        return -1;
    }
    return sock_server;
}
//客户端connect,建立长短链接
int socket_connect(int port, char *host) {
    int sock_id;
    struct sockaddr_in dest_addr;
    if ((sock_id = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        return -1;
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(host);
    if(connect(sock_id, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) < 0) {
        perror("Connect");
        return -1;
    }
    return sock_id;
}

