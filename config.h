#ifndef CONFIG
#define CONFIG

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 10
#define MAX_QUEUES 2
#define BUFFER_SIZE 5
#define OVERWRITE_VALUE 6 //BUFFER_SIZE + 1


typedef enum {
    PRIORITY,
    ROUNDROBIN
} schedulerType;

typedef struct {
    int id;
    int runtime;
    int remainingTime;
    int quantum;
    int creationDate;
    int priority;
    schedulerType schedule;
} Process;

typedef struct {
    void *buffer[OVERWRITE_VALUE];
    int head;
    int tail;
} Queue;

typedef struct {
    Queue queues[MAX_QUEUES];
    int numberQueues;
} Scheduler;



void scheduler_init(Scheduler *s);
int scheduler_add_queue(Scheduler *s);
int insertBuffer(Queue *q, void *item);
void roundRobinScheduler(Queue *queue, Process **processes, int processesQuantity);
void *removeFromBuffer(Queue *q);
void *emptyBuffer(Scheduler *s);


#endif