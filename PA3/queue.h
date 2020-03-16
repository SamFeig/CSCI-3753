#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

// Queue node struct
typedef struct node_type {
	void *value;
} node;

// Queue struct
typedef struct queue_type {
	node *data;
	int size;
	int maxsize;
	int front;
	int back;
} queue;

void enqueue(queue *q, void* element);

void* dequeue(queue *q);

queue* init_queue(queue *q, int size);

void free_queue(queue *q);

int size(queue *q);

int isFull(queue *q);

void pq(queue *q);

#endif