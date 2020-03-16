#include "multi-lookup.h"

int main(int argc, char* argv[]) {
	// Start timing
	struct timeval start, stop;
	gettimeofday(&start, NULL);

	// multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ...]
	// Checking arguments
	if(argc <= 5) {
		fprintf(stderr, "Not enough input arguments. Required: ./multi-lookup <# requester> <# resolver> <requester log> <resolver log> [<data file> ...]\n");
		exit(1);
	} else if(argc > 5 + MAX_INPUT_FILES) {
		fprintf(stderr, "Too many input files specified.\n");
		exit(1);	
	}
	
	// Check output file paths
	if(access(argv[3], F_OK) == -1) {
		fprintf(stderr, "Bad filepath for requester log.\n");
		exit(1);
	}
	if(access(argv[4], F_OK) == -1) {
		fprintf(stderr, "Bad filepath for resolver log.\n");
		exit(1);
	}

	// Checking the number of threads entered
	int num_requester = atoi(argv[1]);
	if(num_requester < 1) {
		fprintf(stderr, "Must have at least 1 requestor thread.\n");
		exit(1);
	} else if (num_requester > MAX_REQUESTER_THREADS) {
		fprintf(stderr, "Number of requester threads can't be more than %d.\n", MAX_REQUESTER_THREADS);
		exit(1);
	}

	int num_resolver = atoi(argv[2]);
	if(num_resolver < 1) {
		fprintf(stderr, "Must have at least 1 resolver thread.\n");
		exit(1);
	} else if (num_resolver > MAX_RESOLVER_THREADS) {
		fprintf(stderr, "Number of resolver threads can't be more than %d.\n", MAX_RESOLVER_THREADS);
		exit(1);
	}

	// Add all input files to a queue for requesters
	queue input_files;
	init_queue(&input_files, MAX_INPUT_FILES);
	for(int i = 5; i < argc; i++) {
		if(access(argv[i], F_OK) == -1) {
			fprintf(stderr, "Bad filepath for input file: %s\n", argv[i]);
		} else {
			enqueue(&input_files, strndup(argv[i], strlen(argv[i])));
		}
	}

	// Check that at least one of the inputted files are valid.
	if(size(&input_files) == 0) {
		fprintf(stderr, "No valid input files specified.");
		exit(1);
	}

	// Mutex and condition variables for shared buffer.
	pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t c_req = PTHREAD_COND_INITIALIZER;
	pthread_cond_t c_res = PTHREAD_COND_INITIALIZER;

	// Lock for input files.
	pthread_mutex_t input_lock = PTHREAD_MUTEX_INITIALIZER;
	// Lock for requester log.
	pthread_mutex_t req_log_lock = PTHREAD_MUTEX_INITIALIZER;
	// Lock for resolver log.
	pthread_mutex_t res_log_lock = PTHREAD_MUTEX_INITIALIZER;


	// Initialize the shared buffer
	int req_done = 0;
	queue buffer;
	init_queue(&buffer, QUEUE_SIZE);

	// Initialize all structs with required data
	request_data req_data;
	req_data.req_log = fopen(argv[3], "w");
	req_data.fileNames = &input_files;
	req_data.buffer = &buffer;
	req_data.m = &m;
	req_data.c_req = &c_req;
	req_data.c_res = &c_res;
	req_data.input_lock = &input_lock;
	req_data.req_log_lock = &req_log_lock;

	resolve_data res_data;
	res_data.res_log = fopen(argv[4], "w");
	res_data.buffer = &buffer;
	res_data.m = &m;
	res_data.c_req = &c_req;
	res_data.c_res = &c_res;
	res_data.res_log_lock = &res_log_lock;
	res_data.req_done = &req_done;

	// Requester and resolver thread arrays
	pthread_t request_tid[num_requester];
	pthread_t resolve_tid[num_resolver];

	// Create threads
	for(int i = 0; i < num_requester; i++) {
		pthread_create(&request_tid[i], NULL, request, &req_data);
	}
	for(int i = 0; i < num_resolver; i++) {
		pthread_create(&resolve_tid[i], NULL, resolve, &res_data);
	}
	

	for(int i = 0; i < num_requester; i++) {
	    pthread_join(request_tid[i], NULL);
	}

	// Set done once all threads finish
	pthread_mutex_lock(&m);
    req_done = 1;
    pthread_mutex_unlock(&m);
    
    for(int i = 0; i < num_resolver; i++) {
	    pthread_join(resolve_tid[i], NULL);
    }

    // Close files and free queue data
    fclose(req_data.req_log);
    fclose(res_data.res_log);
    free_queue(&input_files);
    free_queue(&buffer);

    // Finish timing
    gettimeofday(&stop, NULL);

    // Print stats
    printf("Number of Requestor Threads: %d\n", num_requester);
    printf("Number of Resolver Threads: %d\n", num_resolver);
    printf("Total Run Time: %ld microseconds\n", (stop.tv_sec-start.tv_sec)*1000000 + stop.tv_usec-start.tv_usec);
	return 0;
}

void *request(void* data) {
	request_data* req_data = (request_data*) data;
	// Internal counter of how many files each thread services
	int file_count  = 0;
	while(1) {
		// Lock file queue
		pthread_mutex_lock(req_data->input_lock);
		if(size(req_data->fileNames) > 0) {
			char* fileName = dequeue(req_data->fileNames);
			file_count++;
			pthread_mutex_unlock(req_data->input_lock);
			FILE* file = fopen(fileName, "r");
			char line[MAX_DOMAIN_NAME_LENGTH];
			// Read each file line by line
			while(fgets(line, sizeof(line), file)) {
				// Lock shared buffer
				pthread_mutex_lock(req_data->m);
				// Wait if buffer is full
				while(isFull(req_data->buffer)) {
					pthread_cond_wait(req_data->c_req, req_data->m);
				}
				// Add new line while stripping \n character
				enqueue(req_data->buffer, strndup(strtok(line, "\n"), strlen(line)));
				pthread_mutex_unlock(req_data->m);
				// Signal resolver
				pthread_cond_signal(req_data->c_res);
			}
			fclose(file);
			free(fileName);
		} 
		else {
			pthread_mutex_unlock(req_data->input_lock);
			break;
		}
	}
	// Write to the requester log
	pthread_mutex_lock(req_data->req_log_lock);
	fprintf(req_data->req_log, "Thread %ld serviced %d files.\n", syscall(SYS_gettid), file_count);
	pthread_mutex_unlock(req_data->req_log_lock);
	return 0;
}

void *resolve(void* data) {
	resolve_data* res_data = (resolve_data*) data;
	char *ip = malloc(MAX_IP_LENGTH);
	while(1) {
		// Lock shared buffer.
		pthread_mutex_lock(res_data->m);
		// Run until buffer is empty or all requester threads are done
		if(*(res_data->req_done) == 0 || size(res_data->buffer) > 0) {
			// Wait if buffer is empty
			while(size(res_data->buffer) == 0) {
				pthread_cond_wait(res_data->c_res, res_data->m);
			}
			char* domain = dequeue(res_data->buffer);
			pthread_mutex_unlock(res_data->m);
			// Signal requester
			pthread_cond_signal(res_data->c_req);
			// Lookup IP and write to resolver log
			if(dnslookup(domain, ip, MAX_IP_LENGTH)) {
				// Lock resolver log, write error
				pthread_mutex_lock(res_data->res_log_lock);
				fprintf(stderr, "Bad hostname, IP not found for domain: %s\n", domain);
				fprintf(res_data->res_log, "%s,\n", domain);
				pthread_mutex_unlock(res_data->res_log_lock);
			} 
			else {
				// Lock resolver log, write data
				pthread_mutex_lock(res_data->res_log_lock);
				fprintf(res_data->res_log, "%s,%s\n", domain, ip);
				pthread_mutex_unlock(res_data->res_log_lock);
			}
			free(domain);
		} 
		else {
			pthread_mutex_unlock(res_data->m);
			break;
		}
	}
	free(ip);
	return 0;
}