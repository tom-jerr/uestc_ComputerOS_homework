/*
定义进程控制所需要的函数
定义进程控制块结构
*/

#ifndef Proc_h
#define Proc_h

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
//最大内存
#define MAX_SIZE 20

//定义进程状态
enum proc_status{proc_running = 2,
                 proc_ready = 1,
                 proc_block = 0};

//PCB
typedef struct PCB_type{
    int pid;    //进程id
    int priority;   //进程优先级
    int run_time;   //执行时间
    int round;  //时间片
    int needtime;   //进程完成还需要时间
    int sumtime;    //进程需要的总时间
    char content[20];  //进程名
    int state;  //进程状态
    int size;   //进程大小
    int ppid; //父进程
    struct list_head list;  //进程列表
}PCB, *_PCB;

PCB* initProc(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3);
int readyPro(PCB* newproc, int* size, struct list_head* queue);
int finishedPro(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro);
// int blockedPro(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro);
// int notifyPro(struct list_head* queue1, struct list_head* queue2);
int killed(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro);
void onclock(int* interval, struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro);
void find(int pid, struct list_head* queue1, struct list_head* queue2, struct list_head* queue3);
#endif