#include "config.h"

void scheduler_init(Scheduler *s) {
    s->numberQueues = 0;
}

int scheduler_add_queue(Scheduler *s) {
    if (s->numberQueues >= MAX_QUEUES) {
        return -1;
    }
    Queue *q = &(s->queues[s->numberQueues]);
    q->head = 0;
    q->tail = 0;
    s->numberQueues++;
    return s->numberQueues - 1;
}

int insertBuffer(Queue *q, void *item) {
    if ((q->tail + 1) % OVERWRITE_VALUE == q->head) {
        return -1; // queue is full
    }
   
    if(((Process *)item)->schedule == PRIORITY) {
        // Get the item's priority
        int itemPriority = ((Process *)item)->priority;

        // Find the correct position to insert the item in ascending order of priority
        int insertPos = q->head;
        while (insertPos != q->tail && ((Process *)q->buffer[insertPos])->priority <= itemPriority) {
            insertPos = (insertPos + 1) % OVERWRITE_VALUE;
        }

        // Shifts the items to make room for the new item
        int currentPos = q->tail;
        while (currentPos != insertPos) {
            int prevPos = (currentPos - 1 + OVERWRITE_VALUE) % OVERWRITE_VALUE;
            q->buffer[currentPos] = q->buffer[prevPos];
            currentPos = prevPos;
        }

        // Insert the new item in the correct position
        q->buffer[insertPos] = item;
        q->tail = (q->tail + 1) % OVERWRITE_VALUE;
        return 0;
    }

    // Common to both types of schedulers
    q->buffer[q->tail] = item;
    q->tail = (q->tail + 1) % OVERWRITE_VALUE;
    

    return 0;

}

void roundRobinScheduler(Queue *queue, Process **processes, int processesQuantity) {
    int totalTime = 0;
    int i;

    printf("\nEXECUTANDO...\n\n");

    for (i = 0; i < processesQuantity; i++) 
        printf("\nProcesso %d - Quantum: %d", processes[i]->id, processes[i]->quantum);
        
    while (1) {
        int concluded = 1;
      
        for (i = 0; i < processesQuantity; i++) {
            if (processes[i]->remainingTime > 0) {
                concluded = 0;

                if (processes[i]->remainingTime > processes[i]->quantum) {
                    totalTime += processes[i]->quantum;
                    processes[i]->remainingTime -= processes[i]->quantum;
                    printf("\nExecutando processo %d (Tempo restante: %d)",
                           processes[i]->id, processes[i]->remainingTime);

                    // Requeue the process in the next queue
                    insertBuffer(queue, &(processes[i]));
                } else {
                    totalTime += processes[i]->remainingTime;
                    processes[i]->remainingTime = 0;
                    printf("\nProcesso %d concluído!", processes[i]->id);
                }
            }
        }

        if (concluded)
            break;
    }
}

void *removeFromBuffer(Queue *q) {
    if (q->head == q->tail) {
        return NULL; // queue is empty
    }
    void *item = q->buffer[q->head];
    q->head = (q->head + 1) % OVERWRITE_VALUE;
    return item;
}

int triggerBuffer(Scheduler *s, int queue_idx, void *item) {
    if (queue_idx < 0 || queue_idx >= (s->numberQueues)) {
        return -1; // invalid queue index
    }

    if(((Process *)item)->schedule == ROUNDROBIN){
        Queue *q = &(s->queues[queue_idx]);
        if ((q->tail + 1) % OVERWRITE_VALUE == q->head) return -1; 
        return insertBuffer(q, item);
    }

    return insertBuffer(&(s->queues[queue_idx]), item);
}

void *emptyBuffer(Scheduler *s) {
    int i;
    for (i = 0; i < s->numberQueues; i++) {
        Queue *q = &(s->queues[i]);
        void *item = removeFromBuffer(q);
        if (item != NULL) {
            return item;
        }
    }
    return NULL; // all queues are empty
}


void createPriorityBasedProcesses(int q1, Scheduler s) {
    printf("\n\n---------------------------------- PRIORIDADE ----------------------------------\n\n");
    int shiftCounter = 0;
    int remainingProcesses = MAX_PROCESSES;
    Process *processes[MAX_PROCESSES];

    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i] = (Process *)malloc(sizeof(Process));
        processes[i]->schedule = PRIORITY;
        scanf("%d %d %d %d %d %d",
            &processes[i]->id,
            &processes[i]->creationDate,
            &processes[i]->priority,
            &processes[i]->runtime,
            &processes[i]->remainingTime,
            &processes[i]->quantum);

        int result = triggerBuffer(&s, q1, processes[i]);
        if (result == -1) { // is buffer full?
            shiftCounter++;
            printf("\nEscalonamento %d\n", shiftCounter);

            // Emptying buffer
            void *item;
            while ((item = emptyBuffer(&s)) != NULL) { // Print while make it empty
                Process *process = (Process *)item;
                printf("Process ID: %d, Priority: %d, Scheduler Type: %d\n", process->id, process->priority, process->schedule);
                remainingProcesses--;
                free(process);
            }

            // Add the current process to the buffer again
            result = triggerBuffer(&s, q1, processes[i]);
            printf("Buffer cheio. O processo %d entrará na próxima rodada de alocação.\n", processes[i]->id);
        }
    }

    // Printing of the remaining processes in the last round
    if (remainingProcesses > 0) {
        shiftCounter++;
        printf("\nEscalonamento %d\n", shiftCounter);
        // Emptying buffer
        void *item;
        while ((item = emptyBuffer(&s)) != NULL) {
            Process *process = (Process *)item;
            printf("Process ID: %d, Priority: %d, Scheduler Type: %d\n", process->id, process->priority, process->schedule);
            remainingProcesses--;
            free(process);
        }
    }
}



void createRoundRobinProcesses(int q2, Scheduler s) {
    printf("\n\n---------------------------------- ROUND ROBIN ----------------------------------\n\n");
    Process *processesRR[MAX_PROCESSES];
    Process *processesW[MAX_PROCESSES];
    int processesCount = 0;
    int processesCountW = 0;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        processesRR[i] = (Process *)malloc(sizeof(Process));
        processesRR[i]->schedule = ROUNDROBIN;
        scanf("%d %d %d %d %d %d",
              &processesRR[i]->id,
              &processesRR[i]->creationDate,
              &processesRR[i]->priority,
              &processesRR[i]->runtime,
              &processesRR[i]->remainingTime,
              &processesRR[i]->quantum);

        int result2 = triggerBuffer(&s, q2, processesRR[i]);
        if (result2 == -1) {
            printf("\nBuffer cheio para o processo %d. Buffer de espera acionado.", processesRR[i]->id);
            processesW[processesCountW] = processesRR[i];  // storage process into processesW
            processesCountW++;
        } else {
            printf("\nProcesso %d adicionado.", processesRR[i]->id);
            processesCount++;
        }
    }

    printf("\n");

    roundRobinScheduler(&(s.queues[q2]), processesRR, processesCount);

    if (processesCountW > 0) {
        // Empty buffer
        emptyBuffer(&s);

        // Transfer processesW to processesRR in batches of MAX_BUFFER size
        int startIndex = 0;
        while (startIndex < processesCountW) {
            int endIndex = startIndex + BUFFER_SIZE;
            if (endIndex > processesCountW) {
                endIndex = processesCountW;
            }
            
            processesCount = endIndex - startIndex;
            for (int j = 0; j < processesCount; j++) {
                processesRR[j] = processesW[startIndex + j];
            }
            printf("\nTransferindo processos do buffer de espera para o escalonador...\n\n");
            roundRobinScheduler(&(s.queues[q2]), processesRR, processesCount);
            
            startIndex = endIndex;
        }
    }
}




int main() {
    freopen("inputProcess.txt", "r", stdin);
    freopen("outputProcess.txt", "w", stdout);

    // Setting schedulers queue
    Scheduler s;
    scheduler_init(&s);
    int q1 = scheduler_add_queue(&s);  // Queue for priority scheduling based
    int q2 = scheduler_add_queue(&s); //  Queue for round robin scheduling
     
    // creating and printing processes based on priority
    createPriorityBasedProcesses(q1, s);
    fclose(stdin);

    freopen("inputProcess.txt", "r", stdin);
    createRoundRobinProcesses(q2, s);
   
    return 0;
}