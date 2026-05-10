#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "queue.h"

#define min(a,b) (((a) <= (b)) ? (a) : (b))

struct Process {
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int finish_time;
    int turnaround_time;
    int response_time;
    bool started;
};

// comparator for the qsort function to sort processes based on their arrival time
int compareArrival(const void* a, const void* b){
    struct Process* p1 = (struct Process*) a;
    struct Process* p2 = (struct Process*) b;
    return p1->arrival_time - p2->arrival_time;
}

void calculate_rrs(struct Process p[], int n, int quantum) {
    // sort by arrival time
    qsort(p, n, sizeof(p[0]), compareArrival);

    Queue q;
    initQueue(&q); // set default values

    int current_time = 0;
    int completed = 0;
    int next = 0;

    while (completed < n) {
        
        // as long as there is something to process, keep processing
        while(!isEmpty(&q)){
            int i = dequeue(&q);

            if(!p[i].started){
                p[i].response_time = current_time - p[i].arrival_time;
                p[i].started = true;
            }

            int processing_time = min(p[i].remaining_time, quantum);
            
            p[i].remaining_time -= processing_time;
            current_time += processing_time;
            
            // we prioritize the unprocesseed processes before adding the one that just finished
            while (next < n && p[next].arrival_time <= current_time){
                enqueue(&q, next);
                next++;
            }
            
            // if the process didn't finish yet add it back to the queue 
            if (p[i].remaining_time > 0){
                enqueue(&q, i);
            }

            // if process is finished
            else{
                p[i].finish_time = current_time;
                p[i].turnaround_time = p[i].finish_time - p[i].arrival_time;
                completed++;
            }
        }

        // we reach here, if the cpu is idle
        // so we update the current time to the next process arrival time, so that the cpu can continue processing
        current_time = p[next].arrival_time;

        while (next < n && p[next].arrival_time <= current_time){
            enqueue(&q, next);
            next++;
        }
    }
}

int main() {
    int n = 3;
    int quantum = 2;
    struct Process p[] = {
        {1, 0, 5, 5, 0, 0, 0, false},
        {2, 1, 3, 3, 0, 0, 0, false},
        {3, 2, 8, 8, 0, 0, 0, false}
    };

    

    calculate_rrs(p, n, quantum);

    printf("\nID\tArrival\tBurst\tTurnaround\tResponse\n");
    printf("------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t\t%d\n",
               p[i].id, p[i].arrival_time, p[i].burst_time,
               p[i].turnaround_time, p[i].response_time);
    }

    return 0;
}
