#ifndef CONFIG
#define CONFIG


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 20
#define MAX_NUM_QUEUES 2
#define MAX_QUEUE_SIZE 100


// • Deverá existir uma estrutura de clock_tick que será utilizada para controle do quantum (time slice) de execução dos processos.
// enum de tipo de escalonador

struct clock_tick {
    int totalTime;      // Tempo total do quantum
    int remainingTime;  // Tempo restante do quantum
    int isExpired;      // Flag para indicar se o quantum foi esgotado
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
    void *buffer[MAX_QUEUE_SIZE];
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
    s->num_queues++;
    return s->num_queues - 1;
}

int enqueue(Queue *q, void *item) {
    if ((q->tail + 1) % MAX_QUEUE_SIZE == q->head) {
        return -1; // queue is full
    }

    // Obtém a prioridade do item
    int itemPriority = ((Process *)item)->priority;

    // Encontra a posição correta para inserir o item em ordem crescente de prioridade
    int insertPos = q->head;
    while (insertPos != q->tail && ((Process *)q->buffer[insertPos])->priority <= itemPriority) {
        insertPos = (insertPos + 1) % MAX_QUEUE_SIZE;
    }

    // Desloca os itens para abrir espaço para o novo item
    int currentPos = q->tail;
    while (currentPos != insertPos) {
        int prevPos = (currentPos - 1 + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE;
        q->buffer[currentPos] = q->buffer[prevPos];
        currentPos = prevPos;
    }

    // Insere o novo item na posição correta
    q->buffer[insertPos] = item;
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
    return 0;
}

void *dequeue(Queue *q) {
    if (q->head == q->tail) {
        return NULL; // queue is empty
    }
    void *item = q->buffer[q->head];
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    return item;
}

int scheduler_enqueue(Scheduler *s, int queue_idx, void *item) {
    if (queue_idx < 0 || queue_idx >= s->num_queues) {
        return -1; // invalid queue index
    }
    return enqueue(&(s->queues[queue_idx]), item);
}

void *scheduler_dequeue(Scheduler *s) {
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