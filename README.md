# 使用lish.h中的API进行进程的部分操作

## 1. list.h

- 存放位置：`/usr/src/linux-headers-5.0.0-23/include/linux`；可能因为版本不同而有些许的差异

### 使用到的API

#### 链表的结构

```C
//计算member在type中的位置
#define offsetof(type, member)  (size_t)(&((type*)0)->member)
//根据member的地址获取type的起始地址
#define container_of(ptr, type, member) ({          \
        const typeof(((type *)0)->member)*__mptr = (ptr);    \
    (type *)((char *)__mptr - offsetof(type, member)); })

//链表结构
struct list_head
{
    struct list_head *prev;
    struct list_head *next;
};
```

#### 初始化链表

```C
#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)
    
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void init_list_head(struct list_head *list)
{
    list->prev = list;
    list->next = list;
}
```

#### 添加操作

```C
//从头部添加
static inline void list_add(struct list_head *newNode, struct list_head *head)
{
	__list_add(newNode, head, head->next);
}
#else
extern void list_add(struct list_head *newNode, struct list_head *head);
#endif
//从尾部添加
static inline void list_add_tail(struct list_head *newNode, struct list_head *head)
{
    __list_add(newNode, head->prev, head);
}
```

#### 删除操作

```C
static inline  void __list_del(struct list_head *prev, struct list_head *next)
{
    prev->next = next;
    next->prev = prev;
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = (struct list_head *) 0;
	entry->prev = (struct list_head *) 0;
}
```

#### 判断是否为空

```C
/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}
```

#### 遍历链表操作

```C
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)
```

------

## 2.调用API程序

### 定义进程

```C
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
```

### 进程相关函数

```C
PCB* initProc(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3);	//初始化进程
int readyPro(PCB* newproc, int* size, struct list_head* queue);		//将进程插入first_queue
int finishedPro(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro);	//结束当前进程（currentPro）
int killed(struct list_head* queue1, struct list_head* queue2, struct list_head* queue3, PCB** currentPro);		//杀死当前进程（currentPro）
```

### 主函数调用上述函数

```C
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
    
	//遍历链表
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
```

------

## 3. 函数具体实现过程

### 利用输入的值对进程进行初始化

```C
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
```

### 进程插入第一序列

```C
/进程初始插入第一序列
int readyPro(PCB* newproc, int* size, struct list_head* queue){
    if (*size > MAX_SIZE) {
        printf("It can not ready; the buffer is full!\n\n");
        return -1;
    }

    //第一优先级
    list_add_tail(&newproc->list, queue);
    return 1;
}

```

### 当前进程完成后操作

```C
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
```

### 杀死当前进程

```C
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
```

------

## 4. 结果

### 输入

![](https://github.com/tom-jerr/MyblogImg/raw/main/src/OS/proc_input.png)

### 显示

![](https://github.com/tom-jerr/MyblogImg/raw/main/src/OS/proc_result.png)

