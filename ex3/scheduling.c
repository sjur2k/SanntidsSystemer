#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "scheduling.h"
#include "file_handling.h"
#include "schedulers.h"

volatile int globalTime = 0;

pthread_cond_t timeCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t timeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t taskStateMutex = PTHREAD_MUTEX_INITIALIZER;

const char *taskStateString[] = {
	"idle", "running", "preempted", "finished"};

// File pointer for logging
FILE *logFile;

SchedulerType select_scheduler(const char *arg)
{
	if (strcmp(arg, "FCFS") == 0)
		return FCFS;
	else if (strcmp(arg, "SPN") == 0)
		return SPN;
	else if (strcmp(arg, "RR") == 0)
		return RR;
	else if (strcmp(arg, "HRRN") == 0)
		return HRRN;
	else if (strcmp(arg, "SRT") == 0)
		return SRT;
	else if (strcmp(arg, "FEED") == 0)
		return FEED;
	else
	{
		fprintf(stderr, "Unknown scheduler type: %s\n", arg);
		exit(EXIT_FAILURE);
	}
}

void *timer_function(void *arg)
{
	while (1)
	{
		usleep(timeUnitUs);
		pthread_mutex_lock(&timeMutex);
		globalTime++;
		pthread_cond_broadcast(&timeCond); // Signal all waiting threads
		pthread_mutex_unlock(&timeMutex);
	}
	return NULL;
}

void *task_handler(void *var)
{
	struct Task *task;
	task = (struct Task *)var;
	enum taskState prevTaskState = task->state;

	guarded_printf(logFile, "0: Task %d: initiated in %s \n", task->ID, taskStateString[task->state]);

	while (task->currentRuntime < task->totalRuntime)
	{
		pthread_mutex_lock(&timeMutex);
		pthread_cond_wait(&timeCond, &timeMutex);

		if (task->state != prevTaskState)
		{
			enum taskState taskOldState = prevTaskState;
			prevTaskState = task->state;

			guarded_printf(logFile, "%d: Task %d: %s -> %s, total time worked: %d \n",
						   globalTime - 1, task->ID, taskStateString[taskOldState], taskStateString[task->state], task->currentRuntime);
		}

		if (task->state == running)
			task->currentRuntime++;

		pthread_mutex_unlock(&timeMutex);
	}

	set_task_state(task, finished);

	guarded_printf(logFile, "%d: Task %d: running -> finished, total time worked: %d \n",
				   globalTime, task->ID, task->currentRuntime);

	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <scheduler_type>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int taskCount;

	// Initialize log file
	char fileName[100];
    sprintf(fileName, "log_%s.txt", argv[1]);
	logFile = fopen(fileName, "w");
	if (logFile == NULL)
	{
		perror("Failed to open log file");
		exit(EXIT_FAILURE);
	}

	// Read tasks from the file
	struct Task **tasks = read_tasks_from_file("tasks.txt", &taskCount);

	// Create task threads
	pthread_t threads[taskCount];

	for (int i = 0; i < taskCount; i++)
	{
		if (pthread_create(&threads[i], NULL, task_handler, (void *)tasks[i]) != 0)
		{
			// Handle error: cleanup and exit
			perror("Failed to create thread");

			for (int j = 0; j < i; j++)
			{
				pthread_cancel(threads[j]);
				pthread_join(threads[j], NULL);
			}
			free(tasks);
			return 1;
		}
	}

	sleep(1); // Let everything stabilize
	int schedulerTimeout = 2500;

	// Start the global timer thread
	pthread_t timerThread;
	if (pthread_create(&timerThread, NULL, timer_function, NULL) != 0)
	{
		perror("Failed to create timer thread");
		return 1;
	}

	// Select the scheduler to use based on bash argument
	SchedulerType scheduler = select_scheduler(argv[1]);
	switch (scheduler)
	{
	case FCFS:
		guarded_printf(stdout, "Using First-Come-First-Served scheduler\n");
		first_come_first_served(tasks, taskCount, schedulerTimeout);
		break;
	case SPN:
		guarded_printf(stdout, "Using Shortest Process Next scheduler\n");
		shortest_process_next(tasks, taskCount, schedulerTimeout);
		break;
	case RR:
		guarded_printf(stdout, "Using Round Robin scheduler\n");
		round_robin(tasks, taskCount, schedulerTimeout, QUANTUM);
		break;
	case HRRN:
		guarded_printf(stdout, "Using Highest Response Ratio Next scheduler\n");
		highest_response_ratio_next(tasks, taskCount, schedulerTimeout);
		break;
	case SRT:
		guarded_printf(stdout, "Using Shortest Remaining Time scheduler\n");
		shortest_remaining_time(tasks, taskCount, schedulerTimeout, QUANTUM);
		break;
	case FEED:
		guarded_printf(stdout, "Using Feedback scheduler\n");
		feedback(tasks, taskCount, schedulerTimeout, QUANTUM);
		break;
	default:
		guarded_printf(stderr, "Unknown scheduler type\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < taskCount; i++)
		pthread_join(threads[i], NULL);

	// Print summary of tasks
	guarded_printf(stdout, "Summary of task scheduling \n");
	for (int i = 0; i < taskCount; i++)
	{
		guarded_printf(stdout, "Task with ID %d arrived at time %d, started at time %d and worked for %d out of %d time units \n",
					   tasks[i]->ID, tasks[i]->arrivalTime, tasks[i]->startTime, tasks[i]->currentRuntime, tasks[i]->totalRuntime);
	}

	// Cleanup
	for (int i = 0; i < taskCount; i++)
		free(tasks[i]); // Free each task
	free(tasks);		// Free the task array

	fclose(logFile);
	return 0;
}
