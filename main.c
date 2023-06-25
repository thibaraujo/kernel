#include <stdio.h>
#include <stdlib.h>
#include "config.h"

int main() {
    Scheduler s;
    scheduler_init(&s);

    int q1 = scheduler_add_queue(&s); // fila para o escalonamento por prioridade
    int q2 = scheduler_add_queue(&s); // fila para o escalonamento round robin

    Process *processes[20]; 

    srand(time(NULL)); 

    for (int i = 0; i < 20; i++) {
        processes[i] = (Process *)malloc(sizeof(Process));

        processes[i]->id = (12345 + i);
        processes[i]->creationDate = i;
        processes[i]->priority = rand() % 50;
        processes[i]->time_left = rand() % 50;
        processes[i]->running = rand() % 30;
        processes[i]->schedulerType = 1; 

        int result = -1;
        result = scheduler_enqueue(&s, q1, processes[i]); 
        printf("result: %d", result);
        if (result == -1) printf("Error selecting process scheduler  %d.\n", processes[i]->id);
    }

    void *item;
    while ((item = scheduler_dequeue(&s)) != NULL) {
        Process *process = (Process *)item;
        printf("Process ID: %d, Priority: %d, Scheduler Type: %d\n", process->id, process->priority, process->schedulerType);
    }

    for (int i = 0; i < 4; i++) {
        free(processes[i]);
    }

    return 0;
}
