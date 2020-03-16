#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#include "queue.h"
#include "util.h"

// Limits from the assignment document
#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 10
#define MAX_DOMAIN_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

// Size of the shared buffer
#define QUEUE_SIZE 50

// Requester struct
typedef struct req_data {
	FILE* req_log;					// Requestor Log file
	queue* fileNames;				// Queue
	queue* buffer;					// Shared buffer
	pthread_mutex_t* m;				// Mutex for shared buffer
	pthread_cond_t* c_req;			// Requester condition variable for buffer
	pthread_cond_t* c_res;			// Resolver condition variable for buffer
	pthread_mutex_t* input_lock;	// Mutex for fileNames queue
	pthread_mutex_t* req_log_lock;	// Mutex for writing to the requester log
} request_data;

// Resolver struct
typedef struct res_data {
	FILE* res_log;					// Resolver log file
	queue* buffer;					// Shared buffer
	pthread_mutex_t* m;				// Mutex for shared buffer
	pthread_cond_t* c_req;			// Requester condition variable for buffer
	pthread_cond_t* c_res;			// Resolver condition variable for buffer
	pthread_mutex_t* res_log_lock;	// Mutex for writing to the resolver log
	int* req_done;					// Flag for when all the requesters have finished
} resolve_data;

void *request(void* data);

void *resolve(void* data);

#endif


