/*************************************************************************
	> File Name: socket_connect.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年10月03日 星期三 13时57分42秒
 ************************************************************************/

#include<stdio.h>

//客户端connect,建立长短链接
int socket_connect(char *port1, char *host) {
    int sock_id;
    struct sockaddr_in dest_addr;
    int port = atoi(port1);
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

