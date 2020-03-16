// Basic circular queue as the shared buffer
#include "queue.h"

void enqueue(queue *q, void *element) {
	if(isFull(q)) {
		printf("Error: Queue is full.\n");
		exit(1);
	}
	q->back = (q->back + 1) % q->maxsize;
	q->data[q->back].value = element;
	q->size++;
}

void* dequeue(queue *q) {
	if(q->size == 0) {
		printf("Error: Queue is empty.\n");
		exit(1);
	}
	void *value = q->data[q->front].value;
	q->data[q->front].value = NULL;
	q->front = (q->front + 1) % q->maxsize;
	q->size--;
	return value;
}

// Initialize the queue
queue* init_queue(queue *q, int queue_size) {
	q->data = malloc(queue_size * sizeof(node));
	q->maxsize = queue_size;
	q->front = 0;
	q->back = -1;
	q->size = 0;

	return q;
}

// Free the queue
void free_queue(queue *q) {
	while(q->size > 0) {
		void * val;
		val = dequeue(q);
		free(val);
	}
	free(q->data);
}

int size(queue *q) {
	return q->size;
}

int isFull(queue *q) {
	return (q->size == q->maxsize);
}