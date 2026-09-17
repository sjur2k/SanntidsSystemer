#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "scheduling.h"
#include "schedulers.h"

void set_task_state(struct Task *task, enum taskState taskNewState)
{
    pthread_mutex_lock(&taskStateMutex);
    task->state = taskNewState;
    pthread_mutex_unlock(&taskStateMutex);
}

void wait_for_rescheduling(int quantum, struct Task *task)
{
    int startTime;
    int waitTime;

    pthread_mutex_lock(&timeMutex);
    startTime = globalTime;
    pthread_mutex_unlock(&timeMutex);

    do
    {
        pthread_mutex_lock(&timeMutex);
        pthread_cond_wait(&timeCond, &timeMutex);
        waitTime = globalTime - startTime;
        pthread_mutex_unlock(&timeMutex);
    } while (task->state != finished && waitTime < quantum);

    usleep(timeUnitUs / 100);
}

void round_robin(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    int taskIndex = 0;

    do
    {
        // Skip finished tasks or those that have not arrived yet
        if (tasks[taskIndex]->state == finished || tasks[taskIndex]->arrivalTime > globalTime)
        {
            taskIndex = (taskIndex + 1) % taskCount;
            continue;
        }

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Wait for the quantum interval
        wait_for_rescheduling(quantum, tasks[taskIndex]);

        //  Check if the task is finished
        if (tasks[taskIndex]->state == finished)
        {
        }
        else
        {
            set_task_state(tasks[taskIndex], preempted);
        }

        // Find the next task to run
        taskIndex = (taskIndex + 1) % taskCount;

    } while (globalTime < timeout);
}

// Implement your schedulers here!
void first_come_first_served(struct Task **tasks, int taskCount, int timeout)
{
    int taskIndex = 0;

    // Implement your solution here
    do
    {
        // Wait until task arrives (ASSUMING tasks.txt is sorted by arrivalTime)
        pthread_mutex_lock(&timeMutex);
        while(tasks[taskIndex]->arrivalTime > globalTime && globalTime < timeout){  
            pthread_cond_wait(&timeCond, &timeMutex);
        }
        pthread_mutex_unlock(&timeMutex);

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
        {
            tasks[taskIndex]->startTime = globalTime;
            set_task_state(tasks[taskIndex], running);
        }
        
        pthread_mutex_lock(&timeMutex);
        while (tasks[taskIndex]->state != finished && globalTime < timeout)
        {
            pthread_cond_wait(&timeCond, &timeMutex);
        }
        pthread_mutex_unlock(&timeMutex);
        taskIndex = (taskIndex + 1) % taskCount;

    } while (globalTime < timeout);
}
void shortest_process_next(struct Task **tasks, int taskCount, int timeout)
{
    int shortest_time_idx = 0;
    // Implement your solution here
    do
    {
        pthread_mutex_lock(&timeMutex);

        bool new_task_chosen = false;
        int shortest_time = INT_MAX;
        while(globalTime < timeout){
            for (int idx = 0; idx < taskCount; idx++){
                if (tasks[idx]->arrivalTime <= globalTime    && 
                    tasks[idx]->state != finished           &&
                    tasks[idx]->totalRuntime < shortest_time
                ){
                    shortest_time_idx = idx;
                    shortest_time = tasks[shortest_time_idx]->totalRuntime;
                    new_task_chosen = true;
                }
            }
            if (new_task_chosen){
                break;
            } else {
                pthread_cond_wait(&timeCond, &timeMutex);
            }
        }
        pthread_mutex_unlock(&timeMutex);

        // Set the task state to running
        if (tasks[shortest_time_idx]->startTime == -1)
        {
            tasks[shortest_time_idx]->startTime = globalTime;
            set_task_state(tasks[shortest_time_idx], running);
        }
        
        pthread_mutex_lock(&timeMutex);
        while (tasks[shortest_time_idx]->state != finished && globalTime < timeout)
        {
            pthread_cond_wait(&timeCond, &timeMutex);
        }
        pthread_mutex_unlock(&timeMutex);

    } while (globalTime < timeout);
}

float calculate_R(struct Task *task){
    int s = task->totalRuntime;
    int w = globalTime - task->arrivalTime;
    return (float)((w+s)/(float)s);
}

void highest_response_ratio_next(struct Task **tasks, int taskCount, int timeout)
{
    // Implement your solution here
    int hrr_idx = 0;
    // Implement your solution here
    do
    {
        pthread_mutex_lock(&timeMutex);

        bool new_task_chosen = false;
        int highest_response_ratio = -1;

        while(globalTime < timeout){
            for (int idx = 0; idx < taskCount; idx++){
                float R = calculate_R(tasks[idx]);
                if (tasks[idx]->arrivalTime <= globalTime   && 
                    tasks[idx]->state != finished           &&
                    R > highest_response_ratio
                ){
                    hrr_idx = idx;
                    highest_response_ratio = R;
                    new_task_chosen = true;
                }
            }
            if (new_task_chosen){
                break;
            } else {
                pthread_cond_wait(&timeCond, &timeMutex);
            }
        }
        pthread_mutex_unlock(&timeMutex);

        // Set the task state to running
        if (tasks[hrr_idx]->startTime == -1)
        {
            tasks[hrr_idx]->startTime = globalTime;
            set_task_state(tasks[hrr_idx], running);
        }
        
        pthread_mutex_lock(&timeMutex);
        while (tasks[hrr_idx]->state != finished && globalTime < timeout)
        {
            pthread_cond_wait(&timeCond, &timeMutex);
        }
        pthread_mutex_unlock(&timeMutex);
    } while (globalTime < timeout);
}
void shortest_remaining_time(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    // Implement your solution here
    int taskIndex = 0;
    do
    {
        pthread_mutex_lock(&timeMutex);

        bool new_task_chosen = false;
        int shortest_time_remaining = INT_MAX;
        while(globalTime < timeout){
            for (int idx = 0; idx < taskCount; idx++){
                int time_remaining = tasks[idx]->totalRuntime - tasks[idx]->currentRuntime;
                if (tasks[idx]->arrivalTime <= globalTime   && 
                    tasks[idx]->state != finished           &&
                    time_remaining <= shortest_time_remaining
                ){
                    taskIndex = idx;
                    shortest_time_remaining = time_remaining;
                    new_task_chosen = true;
                }
            }
            if (new_task_chosen){
                break;
            } else {
                pthread_cond_wait(&timeCond, &timeMutex);
            }
        }
        pthread_mutex_unlock(&timeMutex);

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Wait for the quantum interval
        wait_for_rescheduling(quantum, tasks[taskIndex]);

        //  Check if the task is finished
        if (tasks[taskIndex]->state == finished)
        {
        }
        else
        {
            set_task_state(tasks[taskIndex], preempted);
        }

    } while (globalTime < timeout);
}
void feedback(struct Task **tasks, int taskCount, int timeout, int quantum)
{
        // Implement your solution here
    int taskIndex = 0;
    int* priorities = calloc(taskCount,sizeof(int)); // All start with max priority
    do
    {
        pthread_mutex_lock(&timeMutex);
        int highest_priority = INT_MAX; // lower is better (INT_MAX is "unset")
        bool new_task_chosen = false;
        while(globalTime < timeout){
            for (int idx = 0; idx < taskCount; idx++){
                if (tasks[idx]->arrivalTime <= globalTime   && 
                    tasks[idx]->state != finished           &&
                    priorities[idx] < highest_priority
                ){
                    taskIndex = idx;
                    highest_priority = priorities[idx];
                    new_task_chosen = true;
                }
            }
            if (new_task_chosen){
                break;
            } else {
                pthread_cond_wait(&timeCond, &timeMutex);
            }
        }
        pthread_mutex_unlock(&timeMutex);

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Wait for the quantum interval
        wait_for_rescheduling(quantum, tasks[taskIndex]);

        //  Check if the task is finished
        if (tasks[taskIndex]->state == finished)
        {
        }
        else
        {
            set_task_state(tasks[taskIndex], preempted);
            priorities[taskIndex]++;
        }

    } while (globalTime < timeout);
    free(priorities);
}
