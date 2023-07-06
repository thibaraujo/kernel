#ifndef CONFIG
#define CONFIG


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 6
#define MAX_NUM_QUEUES 3
#define MAX_BUFFER_SIZE 3


// • Deverá existir uma estrutura de clock_tick que será utilizada para controle do quantum (time slice) de execução dos processes.
// enum de tipo de escalonador

typedef enum {
    PRIORITY,
    ROUNDROBIN
} schedulerType;

typedef struct  {
    int totalTime;        // Tempo total do processo
    int quantum;         // Quantum
    int remainingTime;  // Tempo restante do quantum
    int isExpired;     // Flag para indicar se o quantum foi esgotado
} clockTick;

typedef struct {
    int id;
    int tempo_exec;
    int remainingTime;
    int quantum;
    int creationDate;
    int priority;
    int schedule;
    clockTick timeControl;
} Process;

typedef struct {
    void *buffer[MAX_BUFFER_SIZE];
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
    if ((q->tail + 1) % MAX_BUFFER_SIZE == q->head) {
        return -1; // queue is full
    }

    if(((Process *)item)->schedule == 0) {
        // Obtém a prioridade do item
        int itemPriority = ((Process *)item)->priority;

        // Encontra a posição correta para inserir o item em ordem crescente de prioridade
        int insertPos = q->head;
        while (insertPos != q->tail && ((Process *)q->buffer[insertPos])->priority <= itemPriority) {
            insertPos = (insertPos + 1) % MAX_BUFFER_SIZE;
        }

        // Desloca os itens para abrir espaço para o novo item
        int currentPos = q->tail;
        while (currentPos != insertPos) {
            int prevPos = (currentPos - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
            q->buffer[currentPos] = q->buffer[prevPos];
            currentPos = prevPos;
        }

        // Insere o novo item na posição correta
        q->buffer[insertPos] = item;
        q->tail = (q->tail + 1) % MAX_BUFFER_SIZE;
        return 0;
    }

    Process **processes_queue = (Process **)q->buffer;
    processes_queue[q->tail] = (Process *)item;
    q->tail = (q->tail + 1) % MAX_BUFFER_SIZE;
    
    return 0;

}

int roundRobinScheduler(Queue *queue, Process **processes, int processesQuantity) {
    int totalTime = 0;
    int i;

    for (i = 0; i < processesQuantity; i++) 
        printf("Processo %d - Quantum: %d\n", processes[i]->id, processes[i]->quantum);
        
    while (1) {
        int concluded = 1;
      
        for (i = 0; i < processesQuantity; i++) {
            if (processes[i]->remainingTime > 0) {
                concluded = 0;

                if (processes[i]->remainingTime > processes[i]->quantum) {
                    totalTime += processes[i]->quantum;
                    processes[i]->remainingTime -= processes[i]->quantum;
                    printf("Executando processo %d (Tempo restante: %d)\n",
                           processes[i]->id, processes[i]->remainingTime);

                    // Reenfileirar o processo na próxima fila
                    enqueue(queue, &(processes[i]));
                } else {
                    totalTime += processes[i]->remainingTime;
                    processes[i]->remainingTime = 0;
                    printf("Processo %d concluído!\n", processes[i]->id);
                }
            }
        }

        if (concluded)
            break;
    }

    return totalTime;
}

void *dequeue(Queue *q) {
    if (q->head == q->tail) {
        return NULL; // queue is empty
    }
    void *item = q->buffer[q->head];
    q->head = (q->head + 1) % MAX_BUFFER_SIZE;
    return item;
}

int scheduler_enqueue(Scheduler *s, int queue_idx, void *item) {
    if (queue_idx < 0 || queue_idx >= (s->num_queues)) {
        return -1; // invalid queue index
    }

    if(((Process *)item)->schedule == 1){
        Queue *q = &(s->queues[queue_idx]);
        if ((q->tail + 1) % MAX_BUFFER_SIZE == q->head) return -1; 
        return enqueue(q, item);
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

// int schedulerDetermin(Process *info) {
//     switch (info->schedule) {
//         case 1:
//             return info->priority;
//         case 2:
//            return info->quantum; 
//         default:
//             return info->priority;
//     }
// }

#endif