#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "proc.h"

#define FirstRound  40
#define SecondRound  60
#define Interval 20

int usedpid[10000];
PCB* currentPro;
PCB idlePro;
struct list_head *pos = NULL; 
//初始化进程
PCB* initProc(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3){
    PCB* newproc = (PCB*)malloc(sizeof(PCB));
     //设置该控制块的id值
    int pid;
    printf("new process pid:\n");
    scanf("%d",&pid);
    for (int i = 0; i < 10000 ; i++){
        if (usedpid[i] == pid){
            char* str = "This pid is used!\n";
            printf("%s",str);
            return NULL;
        }
    }
    usedpid[pid] = pid;
    newproc->pid = pid;

    //设置该控制块的其他值
    printf("new process size:\n");
    scanf("%d",&newproc->size);
    printf("new process content:\n");
    scanf("%s",newproc->content);
    printf("new process sumtime:\n");
    scanf("%d",&newproc->sumtime);

    //设置进程父进程
    int parentid;
    printf("new process parent pid:\n");
    scanf("%d",&parentid);
    newproc->ppid = parentid;
    printf("This process is finished!\n\n");

    newproc->round = FirstRound;
    newproc->run_time = 0; newproc->needtime = newproc->sumtime - newproc->run_time;
    newproc->state = 1;
    newproc->priority = 3;
    return newproc;
}

//寻找进程
void find(int pid, struct list_head* queue1, struct list_head* queue2, struct list_head* queue3){
    int index = -1;
    for (int i = 0; i < 10000 ; i++){
        if (usedpid[i] == pid){
            index = usedpid[i];
        }
    }
    if (index!=-1){
        struct list_head *pos1 = NULL; 
        struct list_head *pos2 = NULL; 
        struct list_head *pos3 = NULL; 

        list_for_each(pos1,queue1)
        { 
            PCB *st=list_entry(pos1,PCB,list); 
            if (st->ppid == pid) st->ppid = -1; 
        } 
        list_for_each(pos2,queue2)
        { 
            PCB *st=list_entry(pos2,PCB,list); 
            if (st->pid == pid) st->ppid = -1; 
        } 
        list_for_each(pos3,queue3)
        { 
            PCB *st=list_entry(pos3,PCB,list); 
            if (st->pid == pid) st->ppid = -1; 
        } 
    }
}

//进程初始插入第一序列
int readyPro(PCB* newproc, int* size, struct list_head* queue){
    if (*size > MAX_SIZE) {
        printf("It can not ready; the buffer is full!\n\n");
        return -1;
    }

    //第一优先级
    list_add_tail(&newproc->list, queue);
    return 1;
}

//当前进程完成后状态
int finishedPro(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro){
    if (*currentPro == &idlePro){
        return 0;
    }
    // //将以结束进程为父进程的进程的ppid改为-1
    // find((currentPro)->pid, queue1, queue2, queue3);
    *currentPro = NULL;
    PCB *neproc = NULL;
    if (!list_empty(queue1)){
        list_for_each(pos,queue1){
		    neproc = list_entry(pos,PCB,list); 
		    break; 
        }
		//printf( "\t new process:pid(%d), runtime(%d), parentpid(%d)\r\n",neproc->pid,neproc->run_time,neproc->ppid); 
        *currentPro = neproc;
        (*currentPro)->state = 1;
        //printf( "\t current process:pid(%d), runtime(%d), parentpid(%d)\r\n",currentPro->pid,currentPro->run_time,currentPro->ppid);
        list_del(&neproc->list);
        return 1;
    }
    else if (!list_empty(queue2)){
       list_for_each(pos,queue2){
		    neproc = list_entry(pos,PCB,list); 
		    break; 
        }
		
        *currentPro = neproc;
        list_del(&neproc->list);
        return 2;
    }
    else {
        *currentPro = &idlePro;
        return 0;
    }
    return -1;
}

// //阻塞当前进程
// int blockedPro(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro){
//     // if (*currentPro != NULL && *currentPro != &idlePro) 
//     list_add_tail(&((*currentPro)->list), queue3);
//     struct list_head *pos = NULL; 
//     list_for_each(pos,queue3)
// 	{ 
// 		PCB *st=list_entry(pos,PCB,list); 
// 		printf( "\t process:pid(%d), runtime(%d), parentpid(%d)\r\n",st->pid,st->run_time,st->ppid); 
		
// 	} 
//     *currentPro = NULL; 
//     PCB *neproc = NULL;
//     if (!list_empty(queue1)){
//         list_for_each(pos,queue1){
// 		    neproc = list_entry(pos,PCB,list); 
// 		    break; 
//         }
// 		//printf( "\t new process:pid(%d), runtime(%d), parentpid(%d)\r\n",neproc->pid,neproc->run_time,neproc->ppid); 
//         *currentPro = neproc;
//         //printf( "\t current process:pid(%d), runtime(%d), parentpid(%d)\r\n",currentPro->pid,currentPro->run_time,currentPro->ppid);
//         list_del(&neproc->list);
//         return 1;
//     }
//     else if (!list_empty(queue2)){
//        list_for_each(pos,queue2){
// 		    neproc = list_entry(pos,PCB,list); 
// 		    break; 
//         }
		
//         *currentPro = neproc;
//         list_del(&neproc->list);
//         return 2;
//     }
//     else {
//         *currentPro = &idlePro;
//         return 0;
//     }
//     return -1;
// }

// //唤醒阻塞进程
// int notifyPro(struct list_head* queue_block, struct list_head* queue_first){
//     while (list_empty(queue_block)){
//         PCB *neproc = NULL;
//         list_for_each(pos,queue_block){
// 		    neproc = list_entry(pos,PCB,list);    
// 		    break; 
//         }
//         list_add_tail(&neproc->list, queue_first);
//         list_del(&neproc->list);
//     }
//     return 1;
// }

//杀死进程
int killed(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro){    
    // if (*currentPro != NULL && *currentPro != &idlePro) 
    //     list_add_tail(&(*currentPro)->list, queue3);
    *currentPro = NULL; 
    PCB *neproc = NULL;
    if (!list_empty(queue1)){
        list_for_each(pos,queue1){
		    neproc = list_entry(pos,PCB,list); 
		    break; 
        }
		//printf( "\t new process:pid(%d), runtime(%d), parentpid(%d)\r\n",neproc->pid,neproc->run_time,neproc->ppid); 
        *currentPro = neproc;
        (*currentPro)->state = 1;
        //printf( "\t current process:pid(%d), runtime(%d), parentpid(%d)\r\n",currentPro->pid,currentPro->run_time,currentPro->ppid);
        list_del(&neproc->list);
        return 1;
    }
    else if (!list_empty(queue2)){
       list_for_each(pos,queue2){
		    neproc = list_entry(pos,PCB,list); 
		    break; 
        }
		
        *currentPro = neproc;
        (*currentPro)->state = 1;
        list_del(&neproc->list);
        return 2;
    }
    else {
        *currentPro = &idlePro;
        return 0;
    }
    return -1;
}

//时钟调度
// void onclock(int* interval, struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro){
//     if (*currentPro == &idlePro) {
//         *currentPro = NULL; 
//         PCB *neproc = NULL;
//         if (!list_empty(queue1)){
//                 list_for_each(pos,queue1){
//                 neproc = list_entry(pos,PCB,list); 
//                 break; 
//             }
//             //printf( "\t new process:pid(%d), runtime(%d), parentpid(%d)\r\n",neproc->pid,neproc->run_time,neproc->ppid); 
//             *currentPro = neproc;
//             //printf( "\t current process:pid(%d), runtime(%d), parentpid(%d)\r\n",currentPro->pid,currentPro->run_time,currentPro->ppid);
//             list_del(&neproc->list);
//         }
//         else if (!list_empty(queue2)){
//             list_for_each(pos,queue2){
//                 neproc = list_entry(pos,PCB,list); 
//                 break; 
//             }
            
//             *currentPro = neproc;
//             list_del(&neproc->list);
//         }
//         else {
//             *currentPro = &idlePro;
//         }
//         return;
//     }
    
//     if (!list_empty(queue1) && (*currentPro)->round == SecondRound){
//         *currentPro = NULL; 
//         PCB *neproc = NULL;
       
//         list_for_each(pos,queue1){
//             neproc = list_entry(pos,PCB,list); 
//             break; 
//         }
//         //printf( "\t new process:pid(%d), runtime(%d), parentpid(%d)\r\n",neproc->pid,neproc->run_time,neproc->ppid); 
//         *currentPro = neproc;
//         //printf( "\t current process:pid(%d), runtime(%d), parentpid(%d)\r\n",currentPro->pid,currentPro->run_time,currentPro->ppid);
//         list_del(&neproc->list);
//         return ;
//     }

//     (*currentPro)->run_time += Interval;
//     (*currentPro)->needtime = (*currentPro)->sumtime - (*currentPro)->run_time;
//     if ((*currentPro)->round == FirstRound && (*currentPro)->run_time >= FirstRound || (*currentPro)->needtime <= 0){
//         (*currentPro)->needtime = (*currentPro)->sumtime + (*currentPro)->run_time;
//         (*currentPro)->run_time -= Interval;
//         (*currentPro)->round = SecondRound;
//         list_add_tail(&((*currentPro)->list), queue2);
//         *currentPro = (PCB*)malloc(sizeof(PCB)); 
//         PCB *neproc = NULL;
//         if (!list_empty(queue1)){
//                 list_for_each(pos,queue1){
//                 neproc = list_entry(pos,PCB,list); 
//                 break; 
//             }
//             //printf( "\t new process:pid(%d), runtime(%d), parentpid(%d)\r\n",neproc->pid,neproc->run_time,neproc->ppid); 
//             *currentPro = neproc;
//             //printf( "\t current process:pid(%d), runtime(%d), parentpid(%d)\r\n",currentPro->pid,currentPro->run_time,currentPro->ppid);
//             list_del(&neproc->list);
//         }
//         else if (!list_empty(queue2)){
//             list_for_each(pos,queue2){
//                 neproc = list_entry(pos,PCB,list); 
//                 break; 
//             }
            
//             *currentPro = neproc;
//             list_del(&neproc->list);
//         }
//         else {
//             *currentPro = &idlePro;
//         }
//     }

//     else if((*currentPro)->round == SecondRound && (*currentPro)->run_time >= SecondRound || (*currentPro)->needtime <= 0){
//         (*currentPro)->needtime = (*currentPro)->sumtime + (*currentPro)->run_time;
//         (*currentPro)->run_time -= Interval;
//         list_add_tail(&(*currentPro)->list, queue2);
//         *currentPro = NULL; 
//         PCB *neproc = NULL;
//         if (!list_empty(queue1)){
//                 list_for_each(pos,queue1){
//                 neproc = list_entry(pos,PCB,list); 
//                 break; 
//             }
//             //printf( "\t new process:pid(%d), runtime(%d), parentpid(%d)\r\n",neproc->pid,neproc->run_time,neproc->ppid); 
//             *currentPro = neproc;
//             //printf( "\t current process:pid(%d), runtime(%d), parentpid(%d)\r\n",currentPro->pid,currentPro->run_time,currentPro->ppid);
//             list_del(&neproc->list);
//         }
//         else if (!list_empty(queue2)){
//             list_for_each(pos,queue2){
//                 neproc = list_entry(pos,PCB,list); 
//                 break; 
//             }
            
//             *currentPro = neproc;
//             list_del(&neproc->list);
//         }
//         else {
//             *currentPro = &idlePro;
//         }
//     }
// }