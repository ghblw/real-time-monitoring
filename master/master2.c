/*************************************************************************
	> File Name: master_test.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年11月13日 星期二 19时51分16秒
 ************************************************************************/

#include <pthread.h>
#include "/home/yuxiaowei/real-time-monitoring/common/head.h"
#include "../common/common.c"
#define INS 5
#define PORT1 1080
#define PORT_DATA 8900
#define PORT_CLT 8732
#define PORT_WARN 9876
int vis[1000] = {0};
pthread_mutex_t mutex[10];
void *func(void *);

int queue[INS + 1] = {0};

typedef struct Node {
    struct sockaddr_in addr;//地址结构体
    struct Node *next;
    int data;
    int port;
} Node, *LinkedList;

LinkedList linkedlist[INS + 5];

struct mypara{
    char *s;
    int num;
};

FILE *log1[INS + 1];

Node insert(LinkedList head, Node *node, int index) {
    Node *p, ret;
    p = &ret;
    ret.data = 0;
    ret.next = head;
    while (p->next && index) {
        p = p->next;
        --index;
    }
    if (index == 0) {
        node->next = p->next;
        p->next = node;
        ret.data = 1;
    }
    return ret;
}

void output(LinkedList head, int num) {
    Node *p = head;
    char logfile[20];
    sprintf(logfile, "%d.log", num);
    log1[num] = fopen(logfile, "w+");
    printf("*******线程%d*******\n", num);
    while (p) {
        printf("%s", inet_ntoa(p->addr.sin_addr));
        fprintf(log1[num], "%s", inet_ntoa(p->addr.sin_addr));
        if (p->next) {
            printf("\n");
            fprintf(log1[num], "%s", "\n");
        }
        p = p->next;
    }
    printf("\n");
}

void clear(LinkedList head) {
    Node *p, *q;
    p = head;
    while (p) {
        q = p->next;
        free(p);
        p = q;
    }
    return ;
}

int find_min(int N, int *arr) {
    int *min = arr;
    int ans = 0;
    for (int i = 0; i < N; ++i) {
        if (*(arr + i) < *min) {
            min = arr + i;
            ans = i;
        }
    }
    return ans;
}

void *warning(void *argv) {
    int sockfd = socket_create(PORT_WARN);
    struct sockaddr_in clt_addr;
    socklen_t len = sizeof(clt_addr);
    while (1) {
        char buffer[MAXN + 5];
        int sockfd1;
        if ((sockfd1 = accept(sockfd, (struct sockaddr *) &clt_addr, &len)) < 0) {
            perror("warning Accept");
        }
        recv(sockfd1, buffer, MAXN, 0);
        strlen(buffer) && printf("[警告信息]%s\n", buffer);
        FILE *fd = fopen("warning.log", "a+");
        fwrite(buffer, sizeof(char), strlen(buffer), fd);
        fclose(fd);
        close(sockfd1);
    }
    close(sockfd);
}

int main() {
    int pid;
    pid = fork();
    if (pid > 0) {
        FILE *fp = fopen("/etc/master_yxw.pid", "w");
        fprintf(fp,"%d", pid);
        fclose(fp);
        exit(0);
    }
    //freopen("/dev/null", "w", stdout);
    // freopen("/dev/null", "w", stderr);
    pthread_t t[INS + 1];//存储线程ID
    struct mypara para[INS + 5];//每个线程有一个独立的数据区，不会发生冲突
    
    //初始化
    for (int i = 0; i < 5; i++) {
        pthread_mutex_init(&mutex[i], NULL);   
    }
    int start = 40, finish = 50, port = 1000;//应该从配置文件读取
    for (int i = start; i <= finish; i++) {
        char IP[100] = "192.168.1.";
        sprintf(IP + strlen(IP), "%d", i);
        Node *p, ret;
        int sub = find_min(INS, queue);
        p = (Node *)malloc(sizeof(Node));
        p->addr.sin_family = AF_INET;
        p->addr.sin_port = htons(port);
        p->addr.sin_addr.s_addr = inet_addr(IP);
        p->next = NULL;
        
        ret = insert(linkedlist[sub], p, queue[sub]);
        queue[sub]++;
        linkedlist[sub] = ret.next;
        //output(linkedlist[sub], sub);
    }
    output(linkedlist[0], 0);
    for (int i = 0; i < INS; ++i) {
        para[i].s = "Hello world!";
        para[i].num = i;//用来标识线程
        if (pthread_create(&t[i], NULL, func, (void *)&para[i]) == -1) {//线程ID（只有在进程环境下有意义）、属性、函数、函数参数（多个参数用结构体），创建结束后值执行结束时间不确定
            printf("error\n");
            exit(1);
        }
        printf("Current pthread id = %ld \n", t[i]);
    }
    pthread_create(&t[5], NULL, warning, (void *)&para[5]);
    int sockfd;
    struct sockaddr_in client_addr;
    sockfd = socket_create(PORT1);//心跳监听套接字
    socklen_t clt_len = sizeof(client_addr);
    while (1) {
        int sockfd2;
        if ((sockfd2 = accept(sockfd, (struct sockaddr *) &client_addr, &clt_len)) < 0) {
            perror("Accept");
        }
        //哈希或遍历，如果存在节点则不需要添加，如果不存在节点则需要添加
        char *IP = inet_ntoa(client_addr.sin_addr);
        printf("[心跳连接]%s\n", IP);
        char str[15];
        strncpy(str, IP + 10, strlen(IP));
        int id = atoi(str);
        if (vis[id]) {
            printf("[判重]%s已存在\n", IP);
            close(sockfd2);
            continue;
        }
        vis[id] = 1;
        Node *p, ret;
        int sub = find_min(INS, queue);
        p = (Node *)malloc(sizeof(Node));
        p->addr = client_addr;
        p->next = NULL;
        pthread_mutex_lock(&mutex[sub]);
        ret = insert(linkedlist[sub], p, queue[sub]);
        queue[sub]++;
        linkedlist[sub] = ret.next;
        printf("插入新节点成功！当前链表元素数量为%d\n", queue[sub]);
        pthread_mutex_unlock(&mutex[sub]);
        //output(linkedlist[sub], sub);
        close(sockfd2);//心跳连接，连上就可以关掉
    }
    //需要等待子进程结束，否则主线程会先结束，整个进程结束
    pthread_join(t[0], NULL);
    pthread_join(t[1], NULL);
    pthread_join(t[2], NULL);
    pthread_join(t[3], NULL);
    pthread_join(t[4], NULL);

    for (int i = 0; i < INS; ++i) {
        printf("%d ", queue[i]);
        printf(" ... ");
        output(linkedlist[i], i);
    }
    printf("\n");
    return 0;
}

Node delete_node(LinkedList *head, char *IP) {
    Node ret, *p, *q;
    ret.data = 0;
    ret.next = (Node *)head;
    p = &ret;
    while (p->next && (strcmp(IP, inet_ntoa(p->next->addr.sin_addr)) != 0)) {
        p = p->next;
    }
    if (p->next) {
        q = p->next;
        p->next = q->next;
        //printf("我要删的是%s\n",inet_ntoa(q->addr.sin_addr));
        free(q);
        ret.data = 1;
    }
    return ret;
}


Node connect_or_delete(LinkedList *head, int port, char *IP, int num) {
    //连接客户端
    int sockfd;
    Node ret;
    if ((sockfd = socket_connect(port, IP)) < 0) {//sockfd是长链接
        //output(head, num);
        printf("%s不在线，删除\n", IP);
        ret = delete_node((Node **)linkedlist[num], IP);
        queue[num]--;
        ret.data = 0;
        char str[15];
        strncpy(str, IP + 10, strlen(IP));
        int id = atoi(str);
        vis[id] = 0;
    } else {
        printf("%s连接成功，执行任务\n", IP);//业务逻辑，接收客户端传来的文件并写入日志，对每个线程的不同脚本分别命名
        ret.next = linkedlist[num];
        int len;
        for (int id = 100; id <= 105; id++) {
            if (send(sockfd, &id, 4, 0) <= 0) {
                perror("发送长链接send\n");
                break;
            }
            int id2;
            if (recv(sockfd, &id2, 4, 0) <= 0) {
                perror("长链接收响应码recv");
                break;
            }
            if (id2 == 4 * id) {
                printf("不存在\n");
                continue;
            }
            //说明Client端文件夹没有日志文件，继续询问下一种日志文件 
            int sock_data;
            send(sockfd, &id, 4, 0);
            //sleep(5);
            if ((sock_data = socket_connect(PORT_DATA, IP)) < 0) {
                perror("Connect数据连接");
            }//短链接
            else {
                printf("开始接收%s的文件\n", IP);
                int len2;
                char buffer[MAXN + 5];    
                char dir[100] = "./";
                strcat(dir, IP);
                mkdir(dir, 0775);
                switch (id2) {
                    case 200: {
                        strcat(dir, "/cpu.log");
                    } break;
                    case 202: {
                        strcat(dir, "/sys.log");
                    } break;
                    case 204: {
                        strcat(dir, "/mem.log");
                    } break;
                    case 206: {
                        strcat(dir, "/user.log");
                    } break;
                    case 208: {
                        strcat(dir, "/disk.log");
                    } break;
                    case 210: {
                        strcat(dir, "/bad.log");
                    } break;
                }
	            FILE *fd = fopen(dir, "a+");
                while ((len2 = recv(sock_data, buffer, MAXN, 0)) > 0) {
                    printf("%s\n",buffer);
                    fwrite(buffer, sizeof(char), strlen(buffer), fd);
                    memset(buffer, 0, sizeof(buffer));
                }
                printf("接收文件结束\n");
                fclose(fd);
                close(sock_data);//关闭短链接
            }
        }
        close(sockfd);
        ret.data = 1;
    }
    return ret;
}

void *func(void *argv) {
    struct mypara *para;
    para = (struct mypara *) argv;
    Node *p = linkedlist[para->num], ret;
    int ind = 0;
    printf("*******遍历第%d个链表*******\n", para->num);
    while (1) {
        if (p == NULL || queue[para->num] == 0) {
            sleep(5);
            p = linkedlist[para->num];//需要看看链表有没有被更新
            continue;
        }
        char *IP = inet_ntoa(p->addr.sin_addr);
        //int port = ntohs(p->addr.sin_port);
        pthread_mutex_lock(&mutex[para->num]);
        ret = connect_or_delete((Node **)linkedlist[para->num], PORT_CLT, IP, para->num);
        linkedlist[para->num] = ret.next;
        if (ret.data == 0) {
            p = p->next;
            //p = linkedlist[para->num];
        } else {
            if (p->next == NULL) p = linkedlist[para->num];
            else p = p->next;
        }
        pthread_mutex_unlock(&mutex[para->num]);
        printf("<-------线程%d还剩%d个节点------->\n", para->num, queue[para->num]);
    }
    //printf("=======第%d个链表遍历结束=======\n", para->num);
    return NULL;
}
