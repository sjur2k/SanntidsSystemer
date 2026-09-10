# TTK4147 - Real-time Systems  

## Exercise - Scheduling

In this exercise you will get to implement and test different scheduling
algorithms on the RPi. Most schedulers can be categorized as being either
preemptive and non-preemptive, you will get to implement schedulers of both
types. Since implementing an actual scheduler is time consuming, you will be
implementing the scheduling algorithms in a software framework in which
artificial "tasks" will be scheduled. The goal is to emulate the behaviour of a
real scheduler. You can find the framework and one example scheduler in
`scheduling.c` and `schedulers.c`. It is strongly suggested that you read
through these to prepare for the lab.

## Introduction

The framework used for emulating the scheduler is based on the artificial tasks
struct

```c
    struct Task {
        enum taskState state;
        int ID;
        int arrivalTime;
        int startTime;          // Some time unit
        int totalRuntime;       // Some time unit
        int currentRuntime;     // Some time unit
    };
```

which should contain all the information needed by the scheduler. The
`taskState` enumeration contains the states
`{idle, running, preempted, finished}`. We will only consider these artificial
states.

The tasks are run by the `task_handler` function which ensures that each "task"
behaves appropriately and according to its state. Even though the tasks actually
are scheduled by the underlying system, the `task_handler` function only "runs"
when it has the appropriate state.

You can interface and change the state of a task by using the `set_task_state`
function. Do not attempt to otherwise change the values of the task struct.
However, you have full access to reading the values. You should only edit
`schedulers.c` file.

The parameters of the tasks are stored in `tasks.txt` and are dynamically
loaded.

The round-robin scheduling algorithm is implemented as an illustration. You can
use this as a template when implementing your algorithms. Note that all
preemptive schedulers should run at intervals of 10 time units, just in the same
way as the round-robin scheduler.

#### TASK A

As an introduction to this exercise you should read through the code before
testing it, make sure you understand how it works! Use the provided `Makefile`
to create the executable file. Since the Raspberry Pi does not have clang
installed by default, you have to install it (`sudo apt install clang`). Does
the scheduler work as expected?

You should call the executable `./scheduling <scheduler_type>` where scheduler
types are FCFS, SPN, RR, HRRN, SRT, and FEED.

You can use `plot.py` script to visualise the results of scheduling stored in
`log.txt` file, by running `python plot.py --log log_RR.txt` if Round-Robin is
the scheduler.

#### TASK B

You will now implement three non-preemptive schedulers, namely:

1. First-Come-First-Served

2. Shortest Process Next

3. Highest Response Ratio Next

It is recommended to use the example as a template for your implementation. As
you will see, there are only minor differences between the schedulers, and large
parts of the code can be reused.

Illustrate or write down how you expect the tasks to be run by the schedulers.

Implement the schedulers.

Test if the schedulers work as expected. Visualize the output using the script.

PS: You can use the diagram on the lecture slides as a reference for your
results, but there might be some slight deviations.

#### TASK C

You will now implement two preemptive schedulers, namely:

1. Shortest Remaining Time

2. Feedback

Illustrate or write down how you expect the tasks to be run by the schedulers.

Implement the schedulers.

Test if the schedulers work as expected. Visualize the output using the script.
