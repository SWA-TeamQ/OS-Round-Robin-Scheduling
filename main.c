#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "queue.h"

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

int min(int a, int b){
    return a <= b ? a : b;
}

// comparator for the qsort function to sort processes based on their arrival time
int compareArrival(const void* a, const void* b){
    struct Process* p1 = (struct Process*) a;
    struct Process* p2 = (struct Process*) b;
    // sort by arrival time
    int diff = p1->arrival_time - p2->arrival_time;
    // if equal sort by their id
    if(diff == 0) diff = p1->id - p2->id;

    return diff;
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
    int n = 14; 
    int quantum = 3;

    struct Process p[] = {
        // Cluster 1: IDs 1-5 will finish around t=28
        {1, 0, 8, 8, 0, 0, 0, false},   {2, 1, 4, 4, 0, 0, 0, false},
        {3, 2, 9, 9, 0, 0, 0, false},   {4, 3, 5, 5, 0, 0, 0, false},
        {5, 4, 2, 2, 0, 0, 0, false},   

        // EDGE CASE: Huge gap. Previous work ends at t=28.
        {6, 50, 3, 3, 0, 0, 0, false},  // Should have Response 0
        {7, 51, 1, 1, 0, 0, 0, false},  
        {8, 52, 10, 10, 0, 0, 0, false},

        // EDGE CASE: Another gap. Previous batch (6-8) finishes at t=50+3+1+10 = 64.
        {9, 80, 4, 4, 0, 0, 0, false},   // Should have Response 0
        {10, 81, 6, 6, 0, 0, 0, false},  

        // EDGE CASE: Arrival exactly when previous finishes.
        {11, 90, 5, 5, 0, 0, 0, false}, 

        // EDGE CASE: Triple tie at t=120.
        {12, 120, 2, 2, 0, 0, 0, false}, // Should have Response 0
        {13, 120, 7, 7, 0, 0, 0, false}, 
        {14, 120, 3, 3, 0, 0, 0, false}
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
