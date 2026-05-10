#include "queue.h"

void initQueue(Queue* q){
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

bool isEmpty(Queue* q){
    return q->count == 0;
}

void enqueue(Queue* q, int process_index){
    if(q->count < MAX_PROCESSES){
        q->rear = (q->rear + 1) % MAX_PROCESSES;
        q->items[q->rear] = process_index;
        q->count++;
    }
    printf("Queue is full");
}


int dequeue(Queue* q){
    if(isEmpty(q)) return -1;

    int process_index = q->items[q->front];
    q->front = (q->front + 1) % MAX_PROCESSES;
    q->count--;
    return process_index;
}