#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

#define MAX_PROCESSES 100

typedef struct {
    int items[MAX_PROCESSES];
    int front;
    int rear;
    int count;
} Queue;

// Function Prototypes (The Contract)
void initQueue(Queue* q);
bool isEmpty(Queue* q);
void enqueue(Queue* q, int process_index);
int dequeue(Queue* q);

#endif
