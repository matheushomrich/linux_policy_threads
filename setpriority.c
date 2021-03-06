#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <linux/sched.h>

sem_t sem_bin;
sem_t mutex;
int counter = 0;
volatile int running = 1;
char *buffer;
int thread_count = 0;
int buffer_index = 0;
int buffer_size = 0;
char letters[26] = {'Z', 'Y', 'X', 'W', 'V', 'U', 'T', 'S', 'R', 'Q', 'P', 'O', 'N', 'M', 'L', 'K', 'J', 'I', 'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'};

void *run(void *data)
{
	char * c = (char *)data;

	counter += 1;
	sem_wait(&mutex);
	sem_wait(&mutex);
	if (thread_count == counter) {
		sem_post(&mutex);
	}
	
	while(buffer_index < buffer_size) {
		sem_wait(&sem_bin);
		buffer[buffer_index] = *c;
		buffer_index += 1;
		sem_post(&sem_bin);
	}

	return 0;
}

void print_sched(int policy)
{
	//int priority_min, priority_max;

	switch(policy){
		case SCHED_DEADLINE:
			//printf("SCHED_DEADLINE");
			break;
		case SCHED_FIFO:
			//printf("SCHED_FIFO");
			break;
		case SCHED_RR:
			//printf("SCHED_RR");
			break;
		case SCHED_NORMAL:
			//printf("SCHED_OTHER");
			break;
		case SCHED_BATCH:
			//printf("SCHED_BATCH");
			break;
		case SCHED_IDLE:
			//printf("SCHED_IDLE");
			break;
		default:
			printf("unknown\n");
	}
	// priority_min = sched_get_priority_min(policy);
	// priority_max = sched_get_priority_max(policy);
	//printf(" PRI_MIN: %d PRI_MAX: %d\n", priority_min, priority_max);
}

int setpriority(pthread_t *thr, int newpolicy, int newpriority)
{
	int policy, ret;
	struct sched_param param;

	if (newpriority > sched_get_priority_max(newpolicy) || newpriority < sched_get_priority_min(newpolicy)){
		//printf("Invalid priority: MIN: %d, MAX: %d", sched_get_priority_min(newpolicy), sched_get_priority_max(newpolicy));
		return -1;
	}

	pthread_getschedparam(*thr, &policy, &param);
	print_sched(policy);

	param.sched_priority = newpriority;
	ret = pthread_setschedparam(*thr, newpolicy, &param);
	if (ret != 0)
		perror("perror(): ");

	pthread_getschedparam(*thr, &policy, &param);
	print_sched(policy);

	return 0;
}

int main(int argc, char **argv)
{
	int thread_number;
	int policy = SCHED_RR;

	if (argc < 2){
		printf("usage: ./%s <execution_time>\n\n", argv[0]);
		return 0;
	}
	
	// printf("Thread number: %s\n", argv[1]);
	// printf("Buffer size: %s\n", argv[2]);
	// printf("Scheduler policy: %s\n", argv[3]);
	// printf("Priority: %s\n", argv[4]);

	thread_count = atoi(argv[1]);
	sem_init(&mutex, 0, thread_count);
	sem_init(&sem_bin, 0, 1); 
	buffer_size = atoi(argv[2]);
	buffer = malloc(sizeof(char) * atoi(argv[2]));

	if (strcmp(argv[3], "SCHED_DEADLINE") == 0) {
		policy = SCHED_DEADLINE;
	} else if (strcmp(argv[3], "SCHED_FIFO") == 0) {
		policy = SCHED_FIFO;
	}  else if (strcmp(argv[3], "SCHED_RR") == 0) {
		policy = SCHED_RR;
	}  else if (strcmp(argv[3], "SCHED_OTHER") == 0) {
		policy = SCHED_OTHER;
	}  else if (strcmp(argv[3], "SCHED_BATCH") == 0) {
		policy = SCHED_BATCH;
	}  else if (strcmp(argv[3], "SCHED_IDLE") == 0) {
		policy = SCHED_IDLE;
	}

	if (policy == -1) {
		return 0;
	}

	thread_number = atoi(argv[1]);
	pthread_t threads[thread_number];

	//TO DO
	//pthread lock semaphore
	pthread_mutex_lock(&mutex)
	for (int i = 0; i < thread_number; i++)
	{
		pthread_create(&threads[i], NULL, run, &letters[26-thread_number]);
		print_sched(policy);
		setpriority(&threads[i], policy, atoi(argv[4]));
	}
	pthread_mutex_unlock(&mutex)
	//TO DO
	//pthread unlock sempahore
	for (int i = 0; i < thread_number; i++)
	{
		pthread_join(threads[i], NULL);
	}

	sem_destroy(&mutex);
	sem_destroy(&sem_bin);
	printf("%s\n", buffer);
	return 0;
}
