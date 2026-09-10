import argparse
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

parser = argparse.ArgumentParser(description='Plot task state timeline.')
parser.add_argument('--tasks', type=str, default='tasks.txt', help='Path to the tasks file')
parser.add_argument('--log', type=str, default='log.txt', help='Path to the log file')
args = parser.parse_args()

# Parsing task data
path = args.tasks
tasks_data = {}
with open(path, "r") as file:
    lines = file.readlines()
    for line in lines[1:]:  # Skip the header line
        parts = line.strip().split()
        if len(parts) == 3:
            task_id = int(parts[0])
            start_time = int(parts[1])
            total_runtime = int(parts[2])
            tasks_data[f"Task {task_id}"] = start_time

# Parsing the log data
# Argument parser for accepting file paths from the console


path = args.log
log_data = []
max_time = 0  # Track the maximum time
with open(path, "r") as file:
    for line in file:
        parts = line.strip().split()
        if len(parts) == 10:
            time = int(parts[0].strip(':'))
            if time > max_time:  # Update max_time if the current time is greater
                max_time = time
            task_id = f"Task {parts[2].strip(':')}"
            old_state = parts[3].strip(',')
            new_state = parts[5].strip(',')
            if "->" in parts:
                log_data.append((time, task_id, old_state, new_state))
   
num_tasks = len(tasks_data)  # Dynamically handle any number of tasks
time_slots = list(range(0, max_time + 10, 1))  # Dynamically set time slots up to max_time

# Initialize states for each task
states = {"not-arrived": -1, "idle": 0, "running": 1, "preempted": 2, "finished": 3}
current_states = {f"Task {i}": "idle" for i in range(num_tasks)}  # Dynamically create state list

# Initialize tasks dictionary to hold task states over time
tasks = {f"Task {i}": [] for i in range(num_tasks)}

# Loop through time slots and assign task states
for time in time_slots:
    for task, state in current_states.items():
        if time >= tasks_data[task]:  # Only plot from start_time
            tasks[task].append(states[state])
        else:
            tasks[task].append(states["not-arrived"])
    
    # Update the states based on log data
    for log in log_data:
        if log[0] == time:
            current_states[log[1]] = log[3]  # Update to the new state

# Define colors for each state
color_map = {-1: 'white', 0: 'blue', 1: 'green', 2: 'red', 3: 'gray'}
state_labels = {-1: "not-arrived", 0: 'Idle', 1: 'Running', 2: 'Preempted', 3: 'Finished'}

# Plot the graph using scatter points for better clarity of states
fig, ax = plt.subplots(figsize=(18, 4 + num_tasks // 2))  # Adjust height dynamically based on number of tasks

# Plot each task's timeline with distinct colors and markers for each state
for i, (task, task_states) in enumerate(tasks.items()):
    for j, state in enumerate(task_states):
        ax.scatter(time_slots[j], i, color=color_map[state], s=100)

# Add labels and title
ax.set_yticks([i for i in range(num_tasks)])  # Dynamically set yticks based on the number of tasks
ax.set_yticklabels(tasks.keys())
ax.set_xlabel('Time')
ax.set_ylabel('Tasks')
ax.set_title('Task State Timeline')

# Add custom legend with task state labels
handles = [mpatches.Patch(color=color_map[state], label=state_labels[state]) for state in states.values()]
plt.legend(handles=handles, title="Task States")

# Set x-axis scale dynamically based on max_time
plt.xticks(time_slots[0::10])  # Show every 10th time slot

# Display the updated plot
#plt.show()

# Save the plot to a file
output_file = args.log + '.png'
plt.savefig(output_file)
print(f"Plot saved to {output_file}")