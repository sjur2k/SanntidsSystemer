#define timeUnitUs 100 * 10 // about 1 ms
#define QUANTUM 10          // 10 time units

extern volatile int globalTime;
extern pthread_cond_t timeCond;
extern pthread_mutex_t timeMutex;
extern pthread_mutex_t taskStateMutex;

typedef enum
{
    FCFS, // First-Come, First-Served
    SPN,  // Shortest Process Next
    RR,   // Round Robin
    HRRN,
    SRT,
    FEED
} SchedulerType;

enum taskState
{
    idle,
    running,
    preempted,
    finished
};

extern const char *taskStateString[];

struct Task
{
    enum taskState state;
    int ID;
    int arrivalTime;    // In some imaginary integer time unit
    int totalRuntime;   // In some imaginary integer time unit
    int startTime;      // In some imaginary integer time unit
    int currentRuntime; // In some imaginary integer time unit
};