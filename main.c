#include <stdio.h>
#include <stdlib.h>
#include "config.h"

// TODO: fila de espera
// TODO: receber arquivo com informações dos processos e gerar output de resultados
// TODO: refatorar código

int main() {
    Scheduler s;
    scheduler_init(&s);

    int q1 = scheduler_add_queue(&s); // fila para o escalonamento por prioridade
    int q2 = scheduler_add_queue(&s); // fila para o escalonamento round robin
    //int awaitQueue = scheduler_add_queue(&s); 

    
    int shiftCounter = 0;

    Process *processes[MAX_PROCESSES]; 

    srand(time(NULL)); 

    // CASO POR PRIORIDADE
    printf("\n\n---------------------------------- PRIORIDADE ----------------------------------\n\n");
    int remainingProcesses = MAX_PROCESSES;
    // Criação dos processos
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i] = (Process *)malloc(sizeof(Process));
        processes[i]->id = (12345 + i);
        processes[i]->creationDate = i;
        processes[i]->priority = rand() % 50;
        processes[i]->schedule = PRIORITY;

        int result = -1;
        result = scheduler_enqueue(&s, q1, processes[i]);
        if (result == -1) {
            shiftCounter++;
            printf("Escalonamento %d\n", shiftCounter);
            
            // Esvaziando buffer
            void *item;
            while ((item = scheduler_dequeue(&s)) != NULL) {
                Process *process = (Process *)item;
                printf("Process ID: %d, Priority: %d, Scheduler Type: %d\n", process->id, process->priority, process->schedule);
                remainingProcesses--;
                free(process);
            }
            result = scheduler_enqueue(&s, q1, processes[i]);
            printf("Buffer cheio. O processo %d entrará na próxima rodada de alocação.\n", processes[i]->id);
        }
    }

    // Impressão dos processos restantes
    while (remainingProcesses > 0) {
        shiftCounter++;
        printf("Escalonamento %d\n", shiftCounter);
        // Esvaziando buffer
        void *item;
        while ((item = scheduler_dequeue(&s)) != NULL) {
            Process *process = (Process *)item;
            printf("Process ID: %d, Priority: %d, Scheduler Type: %d\n", process->id, process->priority, process->schedule);
            remainingProcesses--;
            free(process);
        }
    }



    printf("\n\n---------------------------------- ROUND ROBIN ----------------------------------\n\n");
    // CASO POR ROUND-ROBIN
    Process *processesRR[MAX_PROCESSES]; 
    int processesCount = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
            processesRR[i] = (Process *)malloc(sizeof(Process));
            processesRR[i]->id = (12345 + i);
            processesRR[i]->tempo_exec = i+8;
            processesRR[i]->remainingTime = i+8;
            processesRR[i]->quantum = 2;

            int result2 = -1;
            result2 = scheduler_enqueue(&s, q2, processesRR[i]);
            if (result2 == -1) {
                printf("Buffer cheio.  %d.\n", processes[i]->id);
            } else processesCount++;
    }
    // Executar o escalonamento Round Robin na fila específica
    int tempo_total = roundRobinScheduler(&(s.queues[q2]), processesRR, processesCount);

    printf("Tempo total de execução: %d\n", tempo_total);
    return 0;
}