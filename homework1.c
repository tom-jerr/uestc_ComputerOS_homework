#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
//对每一个进程先初始化并送入队列后；再对另一个进程进行操作
#define FirstRound  40
#define SecondRound  60
#define Interval 20
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

int usedpid[10000];
struct list_head *pos = NULL; 
struct list_head *n = NULL; 

struct list_head first_queue ;
struct list_head second_queue ;
struct list_head block_queue ;

PCB* currentPro;
PCB idlePro;

PCB* initProc(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3);	//初始化进程
int readyPro(PCB* newproc, int* size, struct list_head* queue);		//将进程插入first_queue
int finishedPro(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro);	//结束当前进程（currentPro）
int killed(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro);		//杀死当前进程（currentPro）

int main()
{
	INIT_LIST_HEAD(&first_queue);  //初始化链表（头尾相接，形成空链表循环） 
 
	//判断链表是否为空 
	if(list_empty(&first_queue)){ 
		printf("null\r\n"); 
	}else{ 
		printf("not null\r\n"); 
	} 
	printf("\n"); 
	//批量添加节点 
	printf("add:\r\n");  
	for (int i =0;i<3;i++){
		PCB* st = initProc(&first_queue, &second_queue, &block_queue);
		int s = st->size;
		readyPro(st,&s, &first_queue);
	}

	if (!list_empty(&first_queue)){
        PCB* neproc = list_first_entry(&first_queue, PCB, list);
        currentPro = neproc;
        currentPro->state = 1;
        list_del(&neproc->list);
    }
    else if (!list_empty(&second_queue)){
        PCB* neproc = list_first_entry(&second_queue, PCB, list);
        currentPro = neproc;
        currentPro->state = 1;
        list_del(&neproc->list);
    }
    else {
        currentPro = &idlePro;
    }
 
	//显示执行列表所有节点 
	printf("\n"); 
	printf("show all nodes:\r\n");   
	list_for_each(pos,&first_queue)
	{ 
		PCB *st=list_entry(pos,PCB,list); 
		printf( "\t process:pid(%d), runtime(%d), parentpid(%d), state(%d)\r\n",st->pid,st->run_time,st->ppid, st->state); 
		
	} 
	
	/*
		finished测试，使用currentPro验证
	*/
	printf("\n"); 
	printf("show currentPro:\r\n");

	printf( "\t process:pid(%d), runtime(%d), parentpid(%d), state(%d)\r\n\r\n",currentPro->pid,currentPro->run_time,currentPro->ppid, currentPro->state); 

	int flag1 = finishedPro(&first_queue, &second_queue, &block_queue, &currentPro);
	// printf("\t %d\n",flag1);
	printf("\n"); 
	printf("show currentPro(finished):\r\n");
	if (currentPro == &idlePro)
		printf( "\t process: idlePro \r\n");
	else
		printf( "\t process:pid(%d), runtime(%d), parentpid(%d), state(%d)\r\n\r\n",currentPro->pid,currentPro->run_time,currentPro->ppid, currentPro->state); 


	/*
		killed测试，使用currentPro验证
	*/
	int flag3 = killed(&first_queue,&second_queue, &block_queue, &currentPro);
	printf("\n"); 
	printf("show currentPro(killed):\r\n");
	// printf("\t %d\n", flag3);
	if (currentPro == &idlePro)
		printf( "\t process: idlePro \r\n");
	else
		printf( "\t process:pid(%d), runtime(%d), parentpid(%d), state(%d)\r\n\r\n",currentPro->pid,currentPro->run_time,currentPro->ppid, currentPro->state); 
	return 0;
}



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
    newproc->state = 0;
    newproc->priority = 3;
    return newproc;
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