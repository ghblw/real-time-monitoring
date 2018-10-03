/*************************************************************************
	> File Name: server_info.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年09月29日 星期六 13时49分36秒
 ************************************************************************/

#include<stdio.h>
#include<sys/stat.h>
#include "head.h"
#include "common.h"
#define MAXN 1000

//参数： 命令端口 数据端口
void get_info(int sock_srv1, int sock_srv2) {
    struct sockaddr_in client_addr;//存储客户端信息
    socklen_t clt_len = sizeof(client_addr);
    int new_sock = accept(sock_srv1, (struct sockaddr *) &client_addr, &clt_len);//作为长链接传输标识符
    int pid;
    if ((pid = fork()) < 0)
        printf("Error forking child process\n");
    if (pid == 0) {
        char buffer[MAXN];
        int id,len;
        while ((len = recv(new_sock, &id, 4, 0)) > 0) {                //获取标识符
            send(new_sock, &id, 4, 0);      //返回确认标识符
            int sock_data = accept(sock_srv2, (struct sockaddr *) &client_addr, &clt_len);      //建立短链接传输数据
            int len2;
            char IP[100];
            strcpy(IP, inet_ntoa(client_addr.sin_addr));
            char dir[100] = "/home/yuxiaowei/";
            strcat(dir, IP);    //IP地址为目录
            printf("%s\n",dir);
            mkdir(dir, 0775);
            while ((len2 = recv(sock_data, buffer, MAXN, 0)) > 0) {    //接收数据，分别根据标识符存放到相应文件里
                printf("%s\n",buffer);
                if (id == 100) {
                    strcat(dir, "/cpu.log");
                } else if (id == 101) {
                    strcat(dir,"/disk.log");
                } else if (id == 102) {
                    strcat(dir,"/mem.log");
                }
                FILE *fd;
                fd = fopen(dir, "a+");
                fwrite(buffer, sizeof(char), strlen(buffer), fd);
                fclose(fd);
                memset(buffer, 0, sizeof(buffer));
            }
                close(sock_data);
        }
        close(new_sock);
        exit(0);
    }
    close(new_sock);
}
int main(int argc, char *argv[]) {
    int sock_srv1 = socket_create(argv[1]);//新建命令监听套接字
    int sock_srv2 = socket_create(argv[2]);//新建数据监听套接字
    while(1) {
        get_info(sock_srv1, sock_srv2);
    }
    close(sock_srv1);
    close(sock_srv2);
}
