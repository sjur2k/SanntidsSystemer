// clang -lpthread -std=gnu11 UnboundedPriorityInversion.c
#define _GNU_SOURCE
#define CPU_ID 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <semaphore.h>
#include <inttypes.h>

sem_t sem;
pthread_barrier_t barrier;
pthread_t threadL, threadM, threadH;

void *low_f(void *arg);
void *med_f(void *arg);
void *high_f(void *arg);

int set_cpu(int cpu_number)
{
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void print_pri(pthread_t *thread, char *s)
{
	struct sched_param param;
	int policy;
	pthread_getschedparam(*thread, &policy, &param);
	printf("b:%i ", param.sched_priority);
	printf("%s \n", s);
}

void busy_wait_ms(int ms)
{
	uint64_t pval = 0;
	asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(pval)::"memory");
	uint64_t val = pval;
	while (val < pval + ms * 54054)
	{ // RPi scheduling clock at 54 MHz. Check with sudo dmesg | grep MHz
		asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val)::"memory");
	}
}

void create_and_start_task(pthread_t *thread, void *function, int policy, int priority)
{
	int ret = 0;
	pthread_attr_t tattr;
	struct sched_param param;
	ret += pthread_attr_init(&tattr);
	ret += pthread_attr_getschedparam(&tattr, &param);
	ret += pthread_attr_setschedpolicy(&tattr, policy);
	ret += pthread_attr_setinheritsched(&tattr, PTHREAD_EXPLICIT_SCHED);
	param.sched_priority = priority;
	ret += pthread_attr_setschedparam(&tattr, &param);
	ret += pthread_create(thread, &tattr, function, NULL);
	if (ret != 0)
	{
		printf("Error during task init!");
	}
}

int main()
{
	// Setting up barrier and semaphore
	int threadNum = 4;
	pthread_barrier_init(&barrier, NULL, threadNum);
	sem_init(&sem, 0, 1);

	// TODO set up priority inheritance
	/*
    	Initialize mutex attributes
        Set the mutex protocol in the attribute
        Initialixe the mutex with the attribute
    */
	// Create tasks
	int policy = SCHED_RR;
	create_and_start_task(&threadL, low_f, policy, 1);
	create_and_start_task(&threadM, med_f, policy, 2);
	create_and_start_task(&threadH, high_f, policy, 3);

	// Sleep for 100 ms while other tasks are started
	usleep(100 * 1000);

	printf("-------------------Start test-------------------\n");
	pthread_barrier_wait(&barrier);
	pthread_barrier_destroy(&barrier);

	pthread_join(threadL, NULL);
	pthread_join(threadM, NULL);
	pthread_join(threadH, NULL);

	printf("-------------------End test-------------------\n");

	// Delete semaphore
	sem_destroy(&sem);

	printf("Finished\n");

	exit(EXIT_SUCCESS);
}

void *high_f(void *arg)
{
	set_cpu(CPU_ID);

	print_pri(&threadH, "H0: high priority waiting for sync\n");
	pthread_barrier_wait(&barrier);

	print_pri(&threadH, "H1: high usleep\n");
	usleep(200 * 1000);
	print_pri(&threadH, "H3: high priority thread waits lock\n");
	sem_wait(&sem);
	print_pri(&threadH, "H4: high priority thread has lock\n");
	print_pri(&threadH, "H5: high priority thread runs with lock\n");
	busy_wait_ms(100);
	print_pri(&threadH, "H6: high priority thread runs with lock\n");
	busy_wait_ms(100);
	print_pri(&threadH, "H7: high priority thread return lock\n");
	sem_post(&sem);

	return NULL;
}

void *med_f(void *arg)
{
	set_cpu(CPU_ID);

	print_pri(&threadM, "M0: med priority waiting for sync\n");
	pthread_barrier_wait(&barrier);

	print_pri(&threadM, "M1: med usleep\n");
	usleep(100 * 1000);
	print_pri(&threadM, "M2: med priority thread runs\n");
	busy_wait_ms(100);
	print_pri(&threadM, "M3: med priority thread runs\n");
	busy_wait_ms(100);
	print_pri(&threadM, "M4: med priority thread runs\n");
	busy_wait_ms(100);
	print_pri(&threadM, "M5: med priority thread runs\n");
	busy_wait_ms(100);
	print_pri(&threadM, "M6: med priority thread runs\n");
	busy_wait_ms(100);

	return NULL;
}

void *low_f(void *arg)
{
	set_cpu(CPU_ID);

	print_pri(&threadL, "L0: low priority waiting for sync\n");
	pthread_barrier_wait(&barrier);

	print_pri(&threadL, "L1: low priority thread waits lock\n");
	sem_wait(&sem);
	print_pri(&threadL, "L2: low priority thread has lock\n");
	print_pri(&threadL, "L3: low priority thread runs with lock\n");
	busy_wait_ms(100);
	print_pri(&threadL, "L4: low priority thread runs with lock\n");
	busy_wait_ms(100);
	print_pri(&threadL, "L5: low priority thread runs with lock\n");
	busy_wait_ms(100);
	print_pri(&threadL, "L6: low priority thread return lock\n");
	sem_post(&sem);

	return NULL;
}