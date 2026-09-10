#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "scheduling.h"

static pthread_mutex_t printf_mutex = PTHREAD_MUTEX_INITIALIZER;

void guarded_printf(FILE *output, const char *format, ...)
{
    pthread_mutex_lock(&printf_mutex);

    va_list args;
    va_start(args, format);
    vfprintf(output, format, args);
    va_end(args);

    pthread_mutex_unlock(&printf_mutex);
}

// Function to read tasks from the file dynamically
struct Task **read_tasks_from_file(char *filename, int *taskCount)
{
    FILE *file;
    char line[100];
    int id, arrival_time, total_runtime;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        guarded_printf(stdout, "Error opening file.\n");
        exit(1);
    }

    *taskCount = 0;

    // First, count the number of tasks in the file (ignoring comments)
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == '#' || line[0] == '\n')
        {
            continue;
        }
        (*taskCount)++;
    }

    // Allocate memory for tasks
    struct Task **tasks = (struct Task **)malloc((*taskCount) * sizeof(struct Task *));
    for (int i = 0; i < *taskCount; i++)
    {
        tasks[i] = (struct Task *)malloc(sizeof(struct Task));
    }

    // Reset file pointer to beginning of file
    rewind(file);
    int taskIndex = 0;

    // Read task parameters from the file
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == '#' || line[0] == '\n')
        {
            continue;
        }

        sscanf(line, "%d %d %d", &id, &arrival_time, &total_runtime);

        tasks[taskIndex]->ID = id;
        tasks[taskIndex]->arrivalTime = arrival_time;
        tasks[taskIndex]->totalRuntime = total_runtime;
        tasks[taskIndex]->startTime = -1;
        tasks[taskIndex]->currentRuntime = 0;
        tasks[taskIndex]->state = idle;

        taskIndex++;
    }

    fclose(file);
    return tasks;
}