#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define N 5 // Number of forks and philosophers
pthread_t philosophers[N];
pthread_mutexattr_t fork_attrs[N];
pthread_mutex_t forks[N];
pthread_barrier_t barrier;

void* philosopher(void* arg);

void create_and_start_task(pthread_t *thread, void *function, int policy, int index)
{
	int ret = 0;
	pthread_attr_t tattr;
	struct sched_param param;
	ret += pthread_attr_init(&tattr);
	ret += pthread_attr_getschedparam(&tattr, &param);
	ret += pthread_attr_setschedpolicy(&tattr, policy);
	ret += pthread_attr_setinheritsched(&tattr, PTHREAD_EXPLICIT_SCHED);
	param.sched_priority = index + 1;
	ret += pthread_attr_setschedparam(&tattr, &param);
	ret += pthread_create(thread, &tattr, function, (void*)(long)index);
	if (ret != 0)
	{
		printf("Error during task init!");
	}
}


int main(){

    pthread_barrier_init(&barrier, NULL, N + 1); // N philosophers + 1 main thread

    int policy = SCHED_RR;
    for(int i = 0; i < N; i++){
        pthread_mutexattr_init(&fork_attrs[i]);
	    pthread_mutexattr_setprotocol(&fork_attrs[i], PTHREAD_PRIO_INHERIT);
        pthread_mutex_init(&forks[i],&fork_attrs[i]);
        
        create_and_start_task(&philosophers[i],(void*)philosopher, policy, i);
    }
    usleep(100 * 1000);
    pthread_barrier_wait(&barrier);
    
    for(int i = 0; i < N; i++){
        pthread_join(philosophers[i], NULL);
        pthread_mutex_destroy(&forks[i]);
    }
    pthread_barrier_destroy(&barrier);
    return 0;
}

void* philosopher(void* arg){
    int self_idx = (int)(long)arg;
    int left_fork_idx  = (self_idx + N - 1) % N;
    int right_fork_idx = (self_idx + 1) % N;
    pthread_barrier_wait(&barrier);
    while (1){
        pthread_mutex_lock(&forks[left_fork_idx]);
        usleep(10000); // Helps induce deadlock.
        pthread_mutex_lock(&forks[right_fork_idx]);
        printf("Philosopher %d eating\n", self_idx);

        pthread_mutex_unlock(&forks[left_fork_idx]);
        pthread_mutex_unlock(&forks[right_fork_idx]);
        printf("Philosopher %d thinking\n", self_idx);
    }
}

