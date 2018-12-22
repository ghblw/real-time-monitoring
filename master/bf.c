/*************************************************************************
	> File Name: master_test.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年11月13日 星期二 19时51分16秒
 ************************************************************************/

#include "/home/yuxiaowei/real-time-monitoring/common/head.h"
#include "../common/common.c"
#define INS 1
#define PORT1 1080
int cnt = 0;
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

int main() {
    pthread_t t[INS + 1];//存储线程ID
    struct mypara para[INS + 1];//每个线程有一个独立的数据区，不会发生冲突
    
    //初始化
    int start = 40, finish = 50, port = 8732;//应该从配置文件读取
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
    
    int sockfd;
    struct sockaddr_in client_addr;
    sockfd = socket_create(PORT1);
    socklen_t clt_len = sizeof(client_addr);
    while (1) {
        int sockfd2;
        if ((sockfd2 = accept(sockfd, (struct sockaddr *) &client_addr, &clt_len)) < 0) {
            perror("Accept");
        }
        //哈希或遍历，如果存在节点则不需要添加，如果不存在节点则需要添加
        char *IP = inet_ntoa(client_addr.sin_addr);
        printf("接收%s连接请求\n", IP);
        Node *p, ret;
        int sub = find_min(INS, queue);
        p = (Node *)malloc(sizeof(Node));
        p->addr = client_addr;
        p->next = NULL;
        ret = insert(linkedlist[sub], p, queue[sub]);
        queue[sub]++;
        linkedlist[sub] = ret.next;
        output(linkedlist[sub], sub);
        //close(sockfd2);//心跳连接，连上就可以关掉
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
    ret.next = head;
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
    //printf("删除前\n");
    //output(head, num);
    if (sockfd = socket_connect(port, IP) < 0) {
        //output(head, num);
        printf("%s不在线，删除\n", IP);
        ret = delete_node(linkedlist[num], IP);
        queue[num]--;
    } else {
        printf("%s连接成功，执行任务\n", IP);
        close(sockfd);
    }
    //printf("删除后\n");
    //output(head, num);
    return ret;
}

void *func(void *argv) {
    struct mypara *para;
    para = (struct mypara *) argv;
    Node *p = linkedlist[para->num], ret;
    printf("链表的第一个节点是%s\n", inet_ntoa(p->addr.sin_addr));
    int ind = 0;
    //正常是个死循环，这里只遍历一遍
    printf("*******遍历第%d个链表*******\n", para->num);
    while (p != NULL) {
        char *IP = (char *)malloc(sizeof(char) * 100);
        IP = inet_ntoa(p->addr.sin_addr);
        //printf("这个节点%s\n",IP);
        int port = ntohs(p->addr.sin_port);
        ret = connect_or_delete(linkedlist[para->num], port , IP, para->num);
        linkedlist[para->num] = ret.next;
        if (p->next == NULL) p = linkedlist[para->num];
        else p = p->next;
        printf("-------线程%d还剩%d个节点-------\n", para->num, queue[para->num]);
        output(linkedlist[para->num], para->num);
        sleep(3);
    }
    printf("=======第%d个链表遍历结束=======\n", para->num);
    return NULL;
}
