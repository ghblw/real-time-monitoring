/*************************************************************************
	> File Name: master_test.c
	> Author:yuxiaowei 
	> Mail:1147407739@qq.com 
	> Created Time: 2018年11月13日 星期二 19时51分16秒
 ************************************************************************/

#include "/home/yuxiaowei/real-time-monitoring/common/head.h"
#define INS 5

void *func(void *);

int queue[INS + 1] = {0};

typedef struct Node {
    int data;//地址结构体
    struct Node *next;
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
    log1[num] = fopen(logfile, "w");
    while (p) {
        printf("%d", p->data);
        fprintf(log1[num], "%d", p->data);
        if (p->next) {
            printf(" ");
            fprintf(log1[num], "%s", " ");
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
    freopen("in.in", "r", stdin);
    pthread_t t[INS + 1];//存储线程ID
    struct mypara para[INS + 1];//每个线程有一个独立的数据区，不会发生冲突
    int temp;
    for (int i = 0; i < INS; ++i) {
        para[i].s = "Hello world!";
        para[i].num = i;//用来标识线程
        if (pthread_create(&t[i], NULL, func, (void *)&para[i]) == -1) {//线程ID（只有在进程环境下有意义）、属性、函数、函数参数（多个参数用结构体），创建结束后值执行结束时间不确定
            printf("error\n");
            exit(1);
        }
        printf("Current pthread id = %ld \n", t[i]);
    }
    while (scanf("%d", &temp) != EOF) {//主线程起到平衡的作用，选择节点数量最少的链表插入
        int sub = find_min(INS, queue);
        Node *p, ret;
        p = (Node *)malloc(sizeof(Node));
        p->data = temp;
        p->next = NULL;
        ret = insert(linkedlist[sub], p, queue[sub]);
        queue[sub]++;
        linkedlist[sub] = ret.next;
        printf("调整平衡");
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

void *func(void *argv) {
    struct mypara *para;
    para = (struct mypara *) argv;
    printf("%s %d\n", para->s, para->num);
    Node *p, ret;
    for (int i = para->num; i <= 10; ++i) {
        p = (Node *) malloc(sizeof(Node));
        p->data = i;
        p->next = NULL;
        ret = insert(linkedlist[para->num], p, queue[para->num]);
        queue[para->num]++;
        linkedlist[para->num] = ret.next;
    }
    output(linkedlist[para->num], para->num);
    return NULL;
}
