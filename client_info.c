/*************************************************************************
	> File Name: client_info.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年09月28日 星期五 23时45分36秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include "head.h"
#define MAXN 1000

//参数： IP 命令端口 数据端口
int push_info (int sock_id, char *host,char *PORT2) {
    FILE *p1,*p2,*p3;
    char buffer[MAXN];
    p1 = popen("bash cpulog.sh", "r");
    p2 = popen("bash disklog.sh", "r");
    p3 = popen("bash memlog.sh 80", "r");
    //cpu100 disk101 mem102
    for (int id = 100; id <= 102; id++) {
        send(sock_id, &id, 4, 0);//发送标识符
        recv(sock_id, &id, 4, 0);//接收到确认信息后新建短链接发送文件
        int sock_data;//新建短链接传数据（待封装）
   	    struct sockaddr_in data_addr;
    	int port2 = atoi(PORT2);//数据端口
    	if ((sock_data = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        	perror("Socket");
       		return -1;
    	}
    	data_addr.sin_family = AF_INET;
    	data_addr.sin_port = htons(port2);
    	data_addr.sin_addr.s_addr = inet_addr(host);
    	if(connect(sock_data, (struct sockaddr *) &data_addr, sizeof(struct sockaddr)) < 0) {
        	perror("Connect");
        	return -1;
    	}

        FILE *p;//根据标识符确定文件指针
        int len = 0;
        if (id == 100) {
            p = p1;
        } else if (id == 101) {
            p = p2;
        } else if (id == 102) {
            p = p3;
        }
        while ((len = fread(buffer, 1, MAXN, p)) != 0) {
            printf("%s\n",buffer);
            send(sock_data, buffer, len, 0);    //通过短链接分段发送文件
            printf("send over!\n");
            memset(buffer, 0, sizeof(buffer));
        }
        fclose(p);
        close(sock_data);
    }
    return 1;
}
int main(int argc, char *argv[]) {
    //5s执行一次函数
    int sock_id;//新建长链接传标识符（待封装）
    struct sockaddr_in dest_addr;
    int port1 = atoi(argv[2]);//命令端口
    char *host = argv[1];
    if ((sock_id = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        return -1;
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port1);
    dest_addr.sin_addr.s_addr = inet_addr(host);
    if(connect(sock_id, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) < 0) {
        perror("Connect");
        return -1;
    }
    while(1) {
        push_info(sock_id , argv[1], argv[3]);//额外传入IP和数据端口来建立短链接
        sleep(5);
    }
    close(sock_id);
    return 0;
}
