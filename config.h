#ifndef CONFIG
#define CONFIG


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 20
#define MAX_NUM_QUEUES 2

// • Deverá existir uma estrutura de clock_tick que será utilizada para controle do quantum (time slice) de execução dos processos.
struct clock_tick {
    int total_time;      // Tempo total do quantum
    int remaining_time;  // Tempo restante do quantum
    int is_expired;      // Flag para indicar se o quantum foi esgotado
};

typedef struct {
    int id;
    int creationDate;
    int priority;
    int time_left;
    int running;
    int schedulerType;
} Process;

typedef struct {
    Process *processes[MAX_PROCESSES];
    int head;
    int tail;
} Queue;

typedef struct {
    Queue queues[MAX_NUM_QUEUES];
    int num_queues;
} Scheduler;

void scheduler_init(Scheduler *s) {
    s->num_queues = 0;
}

int scheduler_add_queue(Scheduler *s) {
    if (s->num_queues >= MAX_NUM_QUEUES) {
        return -1;
    }
    Queue *q = &(s->queues[s->num_queues]);
    q->head = 0;
    q->tail = 0;

    // Inicialize os ponteiros de processo da fila como NULL
    for (int i = 0; i < MAX_PROCESSES; i++) {
        q->processes[i] = NULL;
    }

    s->num_queues++;
    return s->num_queues - 1;
}

int enqueue(Queue *q, Process *item) {
    if ((q->tail + 1) % MAX_PROCESSES == q->head % MAX_PROCESSES) return -1;

    if(item->schedulerType == 1) { // escalonamento por prioridade
        while (q->tail != q->head && item->priority <= q->processes[q->tail - 1]->priority) {
            q->tail = (q->tail - 1 + MAX_PROCESSES) % MAX_PROCESSES;
            q->processes[q->tail] = q->processes[(q->tail - 1 + MAX_PROCESSES) % MAX_PROCESSES];
        }

        q->processes[q->tail] = item;
        q->tail = (q->tail + 1) % MAX_PROCESSES;
        printf("----------------%d Assumiu a posicao nova--------------\n", item->id);
    }
}

void printQueue(Scheduler *s, int queue_idx) {
    Queue *q = &(s->queues[queue_idx]);
    int current = q->head;

    while (current != q->tail) {
        Process *process = q->processes[current];
        printf("Process ID: %d\n", process->id);
        printf("Priority: %d\n", process->priority);
        current = (current + 1) % MAX_PROCESSES;
    }
}

void *dequeue(Queue *q) {
    if (q->head == q->tail) return NULL; // queue is empty

    void *item = q->processes[q->head];
    q->head = (q->head + 1) % MAX_PROCESSES;
    return item;
}

int scheduler_enqueue(Scheduler *s, int queue_idx, void *item) {
    if (queue_idx < 0 || queue_idx >= s->num_queues) return -1; // invalid queue index
    return enqueue(&(s->queues[queue_idx]), item);
}

void *scheduler_dequeue(Scheduler *s) {
    printf("entrou aqui no dequeue!\n");
    int i;
    for (i = 0; i < s->num_queues; i++) {
        Queue *q = &(s->queues[i]);
        void *item = dequeue(q);
        if (item != NULL) {
            return item;
        }
    }
    return NULL; // all queues are empty
}

int schedulerDetermin(Process *info) {
    switch (info->schedulerType) {
        case 1:
            return info->priority;
        case 2:
           return info->priority; //o que deve ser analisado no round robin - MUDAR
        default:
            return info->priority;
    }
}

#endif