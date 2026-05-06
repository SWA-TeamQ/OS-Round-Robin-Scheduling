#include <stdio.h>
#include <stdbool.h>

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

void calculate_rrs(struct Process p[], int n, int quantum) {
    int current_time = 0;
    int completed = 0;

    while (completed < n) {
        bool done_something = false;

        for (int i = 0; i < n; i++) {
            if (p[i].arrival_time <= current_time && p[i].remaining_time > 0) {
                done_something = true;

                if (!p[i].started) {
                    p[i].response_time = current_time - p[i].arrival_time;
                    p[i].started = true;
                }

                if (p[i].remaining_time > quantum) {
                    current_time += quantum;
                    p[i].remaining_time -= quantum;
                } else {
                    current_time += p[i].remaining_time;
                    p[i].remaining_time = 0;
                    p[i].finish_time = current_time;
                    completed++;

                    p[i].turnaround_time = p[i].finish_time - p[i].arrival_time;
                }
            }
        }

        if (!done_something) {
            current_time++;
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
