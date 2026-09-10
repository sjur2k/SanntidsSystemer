void set_task_state(struct Task *task, enum taskState taskNewState);
void wait_for_rescheduling(int quantum, struct Task *task);

void round_robin(struct Task **tasks, int taskCount, int timeout, int quantum);
void first_come_first_served(struct Task **tasks, int taskCount, int timeout);
void shortest_process_next(struct Task **tasks, int taskCount, int timeout);
void highest_response_ratio_next(struct Task **tasks, int taskCount, int timeout);
void shortest_remaining_time(struct Task **tasks, int taskCount, int timeout, int quantum);
void feedback(struct Task **tasks, int taskCount, int timeout, int quantum);