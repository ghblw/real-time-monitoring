/*************************************************************************
	> File Name: client1.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年11月18日 星期日 01时41分34秒
 ************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include "../common/common.c"
#define PORT_HRT 1080
#define PORT_LS 8732
#define PORT_DAT 8900
#define INS 10
//参数：Master端IP

pthread_t t[INS + 1];
struct mypara {
    char *s;
    int num;
};
struct mypara para[INS + 1];
void *func1(void *argv) {
    int sock_data, sock_fd;
    sock_fd = socket_create(PORT_LS);//长链接监听套接字
    while (1) {
	    struct sockaddr_in mst_addr1;//存储Master端信息
	    socklen_t mst_len1 = sizeof(mst_addr1);
	    struct sockaddr_in mst_addr2;//存储Master端信息
	    socklen_t mst_len2 = sizeof(mst_addr2);
	    printf("等待主机连接\n");
	    int sock_fd1;
	    if ((sock_fd1 = accept(sock_fd, (struct sockaddr *) &mst_addr1, &mst_len1)) < 0) {
	        perror("Accept1");
	    }//这是长链接
        printf("主机连接成功\n");
        int len, id;
        sock_data = socket_create(PORT_DAT);//短链接监听套接字
        printf("数据监听套接字%d\n",sock_data);
    while ((len = recv(sock_fd1, &id, 4, 0)) > 0) {
        printf("收到主机的标识码%d\n",id);
	    FILE *fd;
	    char dir[100] = "rm";
	    if (id == 100) {
	        strcat(dir," cpu.log");
            printf("准备打开日志文件\n");
	        fd = fopen("./cpu.log", "r");
            printf("打开了日志文件\n");
	    } else if (id == 101) {
	        strcat(dir," sys.log");
	        fd = fopen("./sys.log", "r");
	    } else if (id == 102) {
	        strcat(dir," mem.log");
	        fd = fopen("./mem.log", "r");
	    } else if (id == 103) {
	        strcat(dir," user.log");
	        fd = fopen("./user.log", "r");
	    } else if (id == 104) {
	        strcat(dir," disk.log");
	        fd = fopen("./disk.log", "r");
	    } else if (id == 105) {
	        strcat(dir," bad.log");
	        fd = fopen("./bad.log", "r");
	    }
	    if (fd == NULL) {
            printf("不存在日志文件\n");
		    id *= 4;
		    send(sock_fd1, &id, 4, 0);
		    continue;
	    }
	    id *= 2; 
	    send(sock_fd1, &id, 4, 0);
	    if (recv(sock_fd1, &id, 4, 0) <= 0) continue;
        printf("存在文件，开始传文件\n");
	    //开始传文件	
	    int sock_data1;
	    if((sock_data1 = accept(sock_data, (struct sockaddr *)&mst_addr2, &mst_len2)) < 0) {
	        perror("Accept2");
	    }//短链接
	    int len;
        char buffer[MAXN + 5];
        printf("开始读日志文件\n");
	    while ((len = fread(buffer, 1, MAXN, fd)) != 0) {
		    printf("读日志文件%s\n", buffer);
		    send(sock_data1, buffer, strlen(buffer), 0);
		    memset(buffer, 0, sizeof(buffer));
	    }
	    fclose(fd);
	    close(sock_data1);
        close(sock_data);
	    system(dir);
    }
        printf("Master关闭了连接，等待下一次连接\n");
        close(sock_fd1);
        sleep(5);
    }
}

int main(int argc, char *argv[]) {
    pthread_create(&t[0], NULL,func1, (void *)&para[0]);//等待主机连接
    //pthread_create(&t[1], NULL,func2, (void *)&para[1]);//警报信息监控
    while (1) {//需要改成六个线程同时执行脚本
        printf("发送心跳\n");
        int sock_id = socket_connect(PORT_HRT, argv[1]);
        close(sock_id);//心跳连接
        FILE *p1, *p2, *p3, *p4, *p5, *p6;
        char buffer[MAXN + 5];
        p1 = popen("bash ../script/cpulog.sh", "r");
        p2 = popen("bash ../script/syslog.sh", "r");
        p3 = popen("bash ../script/memlog.sh", "r");
        p4 = popen("bash ../script/userlog.sh", "r");
        p5 = popen("bash ../script/disklog.sh", "r");
        p6 = popen("bash ../script/bad.sh", "r");
        //printf("%s\n",buffer);
        FILE *p;
        for (int id = 100; id <= 105; id++) {
            char dir[100] = "./";
            switch (id) {
                case 100: {
                    p = p1;
                    strcat(dir, "cpu.log");
                } break;
                case 101: {
                    p = p2;
                    strcat(dir, "sys.log");
                } break;
                case 102: {
                    p = p3;
                    strcat(dir, "mem.log");
                } break;
                case 103: {
                    p = p4;
                    strcat(dir, "user.log");
                } break;
                case 104: {
                    p = p5;
                    strcat(dir, "disk.log");
                } break;
                case 105: {
                    p = p6;
                    strcat(dir, "bad.log");
                } break;
            }
            int len;
            FILE *fd = fopen(dir, "a+");
            while ((len = fread(buffer, 1, MAXN, p)) != 0) {
                //printf("%s\n",buffer);
                fwrite(buffer, sizeof(char), strlen(buffer), fd);
                fclose(fd);
                memset(buffer, 0, sizeof(buffer));
            }
        }
        sleep(5);
    }
}
