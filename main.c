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
    int waiting_time;
    int response_time;
    int io_remaining;
    bool started;
    bool in_io;
};

// Utility function
int min(int a, int b) {
    return a <= b ? a : b;
}

// Comparator for qsort
int compareArrival(const void* a, const void* b) {
    struct Process* p1 = (struct Process*) a;
    struct Process* p2 = (struct Process*) b;
    int diff = p1->arrival_time - p2->arrival_time;
    if (diff == 0) diff = p1->id - p2->id;
    return diff;
}

void calculate_rrs_with_io(struct Process p[], int n, int quantum) {
    // Sort initially by arrival
    qsort(p, n, sizeof(p[0]), compareArrival);

    Queue ready_q;
    initQueue(&ready_q);

    int current_time = 0;
    int completed = 0;
    int next_to_arrive = 0;

    printf("\n--- Simulation Trace ---\n");

    while (completed < n) {
        // 1. Move arrived processes to Ready Queue
        while (next_to_arrive < n && p[next_to_arrive].arrival_time <= current_time) {
            enqueue(&ready_q, next_to_arrive);
            next_to_arrive++;
        }

        // 2. Handle Sleeping Processes (I/O)
        for (int i = 0; i < n; i++) {
            if (p[i].in_io) {
                p[i].io_remaining--;
                if (p[i].io_remaining <= 0) {
                    p[i].in_io = false;
                    enqueue(&ready_q, i);
                    printf("[t=%d] Process %d Woke up from I/O\n", current_time, p[i].id);
                }
            }
        }

        // 3. Execution
        if (!isEmpty(&ready_q)) {
            int i = dequeue(&ready_q);

            if (!p[i].started) {
                p[i].response_time = current_time - p[i].arrival_time;
                p[i].started = true;
            }

            int run_time = min(p[i].remaining_time, quantum);
            
            // Logic: If burst is heavy, it triggers I/O after a partial burst
            bool triggers_io = (p[i].remaining_time > 6 && !p[i].in_io);
            if (triggers_io) run_time = 2; 

            p[i].remaining_time -= run_time;
            current_time += run_time;

            if (p[i].remaining_time == 0) {
                p[i].finish_time = current_time;
                p[i].turnaround_time = p[i].finish_time - p[i].arrival_time;
                p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
                completed++;
            } else if (triggers_io) {
                p[i].in_io = true;
                p[i].io_remaining = 5; // Fixed sleep time
                printf("[t=%d] Process %d requested I/O -> Sleeping\n", current_time, p[i].id);
            } else {
                // Check arrivals again during the run time
                while (next_to_arrive < n && p[next_to_arrive].arrival_time <= current_time) {
                    enqueue(&ready_q, next_to_arrive);
                    next_to_arrive++;
                }
                enqueue(&ready_q, i);
            }
        } else {
            current_time++; // IDLE CPU
        }
    }

    // Results Analysis
    float total_turnaround = 0, total_waiting = 0;
    printf("\nID\tArrival\tBurst\tTurnaround\tWait\tResponse\n");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        total_turnaround += p[i].turnaround_time;
        total_waiting += p[i].waiting_time;
        printf("%d\t%d\t%d\t%d\t\t%d\t%d\n",
               p[i].id, p[i].arrival_time, p[i].burst_time,
               p[i].turnaround_time, p[i].waiting_time, p[i].response_time);
    }

    printf("\nAverage Turnaround Time: %.2f\n", total_turnaround / n);
    printf("Average Waiting Time: %.2f\n", total_waiting / n);
}

int main() {
    int n = 14; 
    int quantum = 3;

    struct Process p[] = {
        {1, 0, 8, 8, 0, 0, 0, 0, 0, false, false},   {2, 1, 4, 4, 0, 0, 0, 0, 0, false, false},
        {3, 2, 9, 9, 0, 0, 0, 0, 0, false, false},   {4, 3, 5, 5, 0, 0, 0, 0, 0, false, false},
        {5, 4, 2, 2, 0, 0, 0, 0, 0, false, false},   {6, 50, 3, 3, 0, 0, 0, 0, 0, false, false},
        {7, 51, 1, 1, 0, 0, 0, 0, 0, false, false},  {8, 52, 10, 10, 0, 0, 0, 0, 0, false, false},
        {9, 80, 4, 4, 0, 0, 0, 0, 0, false, false},  {10, 81, 6, 6, 0, 0, 0, 0, 0, false, false},
        {11, 90, 5, 5, 0, 0, 0, 0, 0, false, false}, {12, 120, 2, 2, 0, 0, 0, 0, 0, false, false},
        {13, 120, 7, 7, 0, 0, 0, 0, 0, false, false},{14, 120, 3, 3, 0, 0, 0, 0, 0, false, false}
    };

    calculate_rrs_with_io(p, n, quantum);

    return 0;
}
