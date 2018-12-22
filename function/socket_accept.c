/*************************************************************************
	> File Name: socket_accept.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年10月20日 星期六 16时35分15秒
 ************************************************************************/

#include<stdio.h>

//参数：监听套接字(如果需要客户端信息还需要调整)
int socket_accept(int sock_listen) {
    struct sockaddr_in client_addr;//存储客户端信息
    socklen_t clt_len = sizeof(client_addr);
    int new_sock = accept(sock_srv1, (struct sockaddr *) &client_addr, &clt_len);
    return new_sock;
}
