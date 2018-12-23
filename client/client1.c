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
#define PORT_WARN 9876
#define INS 10
//参数：Master端IP

pthread_t t[INS + 1];
pthread_mutex_t mutex[10];

struct mypara {
    char *s;
    int num;
};
struct mypara para[INS + 1];
int sock_fd, sock_data;
void *send_file(void *argv) {
    while (1) {
	    struct sockaddr_in mst_addr;//存储Master端信息
	    socklen_t mst_len = sizeof(mst_addr);
	    printf("等待主机连接\n");
	    int sock_fd1;
	    if ((sock_fd1 = accept(sock_fd, (struct sockaddr *) &mst_addr, &mst_len)) < 0) {
	        perror("Accept1");
	    }//这是长链接
        printf("主机连接成功\n");
        int len, id, mutex_id;
        while ((len = recv(sock_fd1, &id, 4, 0)) > 0) {
            FILE *fd;
            char dir[100] = " >";
            if (id == 100) {
                strcat(dir," cpu.log");
                fd = fopen("./cpu.log", "r");
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
            mutex_id = id % 10 + 1;
            pthread_mutex_lock(&mutex[mutex_id]);
            if (fd == NULL) {
                //printf("不存在日志文件\n");
                id *= 4;
                send(sock_fd1, &id, 4, 0);
                pthread_mutex_unlock(&mutex[mutex_id]);
                continue;
            }
            id *= 2; 
            send(sock_fd1, &id, 4, 0);
            recv(sock_fd1, &id, 4, 0);
            //printf("存在文件，开始传文件\n");
            //开始传文件	
            int sock_data1;
            sleep(5);
            if((sock_data1 = accept(sock_data, (struct sockaddr *)&mst_addr, &mst_len)) < 0) {
                perror("Accept2");
            }//短链接
            int len;
            char buffer[MAXN + 5];
            while ((len = fread(buffer, 1, MAXN, fd)) != 0) {
                //printf("%s\n", buffer);
                send(sock_data1, buffer, strlen(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
            }
            fclose(fd);
            close(sock_data1);
            system(dir);
            pthread_mutex_unlock(&mutex[mutex_id]);
        }
        printf("Master关闭了连接，等待下一次连接\n");
        close(sock_fd1);
        sleep(5);
    }
}

void *run_script(void *argv) {
    struct mypara *para;
    para = (struct mypara *) argv;
    while (1) {//需要改成六个线程同时执行脚本
        char buffer[MAXN + 5];
        FILE *p;
        int id = para->num, sleep_time;
               char dir[100] = "./";
               switch (id) {
                   case 1: {
                       p = popen("bash ../script/cpulog.sh", "r");
                       strcat(dir, "cpu.log");
                       sleep_time = 5;
                   } break;
                   case 2: {
                       p = popen("bash ../script/syslog.sh", "r");
                       strcat(dir, "sys.log");
                       sleep_time = 60;
                   } break;
                   case 3: {
                       p = popen("bash ../script/memlog.sh 0", "r");
                       strcat(dir, "mem.log");
                       sleep_time = 5;
                   } break;
                   case 4: {
                       p = popen("bash ../script/userlog.sh", "r");
                       strcat(dir, "user.log");
                       sleep_time = 60;
                   } break;
                   case 5: {
                       p = popen("bash ../script/disklog.sh", "r");
                       strcat(dir, "disk.log");
                       sleep_time = 60;
                   } break;
                   case 6: {
                       p = popen("bash ../script/bad.sh", "r");
                       strcat(dir, "bad.log");
                       sleep_time = 5;
                   } break;
               }
               int len;
               pthread_mutex_lock(&mutex[id]);
               FILE *fd = fopen(dir, "a+");
               while ((len = fread(buffer, 1, MAXN, p)) != 0) {
                   printf("%s\n",buffer);
                   fwrite(buffer, sizeof(char), strlen(buffer), fd);
                   memset(buffer, 0, sizeof(buffer));
               }
               fclose(p);
               fclose(fd);
               pthread_mutex_unlock(&mutex[id]);
        sleep(sleep_time);
    }
}

void *warning(void *argv) {
    while (1) {
        FILE *p = popen("bash ../script/alter.sh", "r");
        char buffer[MAXN + 5];
        int sock_wn = socket_connect(PORT_WARN, "192.168.1.76");
        printf("%s\n", buffer);
        fread(buffer, 1, MAXN, p);
        send(sock_wn, buffer, strlen(buffer), 0);
        sleep(2);
    }
}

int main(int argc, char *argv[]) {
    sock_fd = socket_create(PORT_LS);//长链接监听套接字
    sock_data = socket_create(PORT_DAT);//短链接监听套接字
    pthread_create(&t[0], NULL,send_file, (void *)&para[0]);//线程0:等待Master连接，传文件
    pthread_create(&t[7], NULL,warning, (void *)&para[7]);//线程7:发送报警信息
    for (int i = 1; i <= 6; i++) {
        pthread_mutex_init(&mutex[i], NULL);   
    }
    for (int i = 1; i < 7; i++) {//六个线程跑脚本
        para[i].num = i;//存0~5区分六个脚本
        pthread_create(&t[i], NULL, run_script, (void *)&para[i]);
    }
    while (1) {
        int sock_id = socket_connect(PORT_HRT, "192.168.1.76");
        close(sock_id);//心跳连接
        sleep(5);
    }
}
