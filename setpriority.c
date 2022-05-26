#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/sched.h>

volatile int running = 1;
char letters[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

void *run(void *data)
{
	char * c = (char *)data;
	printf("DATA %s", c);
	while (running);

	return 0;
}

void print_sched(int policy)
{
	int priority_min, priority_max;

	switch(policy){
		case SCHED_DEADLINE:
			printf("SCHED_DEADLINE");
			break;
		case SCHED_FIFO:
			printf("SCHED_FIFO");
			break;
		case SCHED_RR:
			printf("SCHED_RR");
			break;
		case SCHED_NORMAL:
			printf("SCHED_OTHER");
			break;
		case SCHED_BATCH:
			printf("SCHED_BATCH");
			break;
		case SCHED_IDLE:
			printf("SCHED_IDLE");
			break;
		default:
			printf("unknown\n");
	}
	priority_min = sched_get_priority_min(policy);
	priority_max = sched_get_priority_max(policy);
	printf(" PRI_MIN: %d PRI_MAX: %d\n", priority_min, priority_max);
}

int setpriority(pthread_t *thr, int newpolicy, int newpriority)
{
	int policy, ret;
	struct sched_param param;

	if (newpriority > sched_get_priority_max(newpolicy) || newpriority < sched_get_priority_min(newpolicy)){
		printf("Invalid priority: MIN: %d, MAX: %d", sched_get_priority_min(newpolicy), sched_get_priority_max(newpolicy));

		return -1;
	}

	pthread_getschedparam(*thr, &policy, &param);
	printf("current: ");
	print_sched(policy);

	param.sched_priority = newpriority;
	ret = pthread_setschedparam(*thr, newpolicy, &param);
	if (ret != 0)
		perror("perror(): ");

	pthread_getschedparam(*thr, &policy, &param);
	printf("new: ");
	print_sched(policy);

	return 0;
}

int main(int argc, char **argv)
{
	int *thread_number;
	int policy = -1;

	if (argc < 2){
		printf("usage: ./%s <execution_time>\n\n", argv[0]);
		return 0;
	}
	
	printf("Thread number: %s\n", argv[1]);
	printf("Buffer size: %s\n", argv[2]);
	printf("Scheduler policy: %s\n", argv[3]);
	printf("Priority: %s\n", argv[4]);

	// char *string_policy = argv[3];{
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
	print("AAAAAAAAAAAAAAAAAA")
	printf("%d", policy);

	*thread_number = atoi(argv[1]);
	while(thread_number > 0) {
		pthread_t thr;
		pthread_create(&thr, NULL, run, &thread_number);
		setpriority(&thr, SCHED_FIFO, 1);
		pthread_join(thr, NULL);
		thread_number -= 1;
	}

	return 0;
}
