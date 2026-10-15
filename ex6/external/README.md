# TTK4147 - Real-time Systems  

## Exercise - AVR32 FreeRTOS and BRTT

Last week you used the AVR32 without an operating system. This week we will use
an operating system called FreeRTOS. It is a small and simple operating system
kernel, with features you expect from a real-time system. Many embedded
microcontrollers are supported, including AVR32 UC3. A description of how
FreeRTOS works, how to use it and an API can be found at  
<http://www.freertos.org/>. 

Remember that the desired properties of a real-time system are consistent and predictable timings, not necessarily fast ones. 

It will be very hard to beat the fast timings from interrupts last week, and not
possible at all with FreeRTOS. When comparing results to the previous exercise,
look at the shape of the distributions and the relative timings between the
response tasks, not the absolute timings. 

As in the previous exercise, boot Windows on the lab PC and login with your username and password. 
Remember to save your histograms for task B, C, D and E and code for approval.
Please write down your answers or your thoughts to the questions throughout the tasks.


## Demo program

FreeRTOS Microchip Studio project is available in this repository. It is very
important that the repository is cloned on the local disk. The default program
does exactly the same as the default program from last week, it blinks an LED
and prints "tick" to the serial console. The difference is that this is
performed in a FreeRTOS task, which will be described below. You should compile
and run the program, and verify that the LED and printing works as expected.

`printf()` can sometimes use a large amount of memory, depending on what you are
printing out. Printing a simple string uses very little, but formatting just a
single integer won’t even run unless the stack size is at least 1024 bytes.
Since this microcontroller only has 64KB memory, it means you can’t start a
whole lot of threads that will use `printf()`. It likely won’t be a problem
today, just be aware that you can increase the stack size if `printf()` is
misbehaving, but you can’t keep doing this forever.

## Creating FreeRTOS tasks

When programming AVR32 directly (like we did last week) there is really no
concept of concurrency. There is only one process, the one running the `main()`
function. The only way to execute code other than what is defined in the
`main()` function is by using interrupts. With FreeRTOS it is possible to create
tasks, and FreeRTOS will schedule which one of these tasks that will run at any
given time. Below is a list for functions you need for creating tasks and run
them in a real time environment:

- `xTaskCreate()`: Creates tasks

- `vTaskStartScheduler()`: Start real-time scheduling, must be called after
  tasks have been created

For details about the functions and their parameters you can to look at the API
on the FreeRTOS webpage (<http://www.freertos.org/a00106.html>), or other
documents you might find online.

Remember you also need to create the function the task is supposed to execute.
When creating a task the stack size can be set by the third parameter to
`xTaskCreate`, and should be no lower than `configMINIMAL_STACK_SIZE` (default
256). The priority should be set to a value higher than `tskIDLE_PRIORITY`, and
no larger than `configMAX_PRIORITIES` (default 8).

#### TASK A

Create two tasks that periodically toggle two LEDs. The first task should toggle
`LED0` every 200ms, and the second task should toggle `LED1` every 500ms. You
should use `vTaskDelay()`, not the busy delay. Make sure the LEDs blink at the
correct frequencies by measuring with a stopwatch. It doesn’t have to be
completely accurate, we’re just checking that everything works as expected.

## Reaction test

We will now run the same test as we did last week, but using FreeRTOS instead of
bare bones AVR32 programming. Since we are now able to create tasks, we can
create one task for each of the three signals we need to respond to. Each of
these tasks should listen to its test pin, and set its response signals as soon
as it receives the test signal from the BRTT.

FreeRTOS uses round-robin scheduling between tasks of equal priority. The
scheduling is preemptive, which means it can interrupt a task while it is
running at any point, where it will swap to a higher priority task or a
different task of the same priority. This occurs whenever a scheduler timer
interrupt occurs (a "tick"), or the current task triggers a reschedule via a
call to functions like `yield`, `delay`, or functions associated with things
like queues and semaphores.

The `FreeRTOSConfig.h` file defines the CPU frequency of the AVR and the tick
frequency. The CPU frequency of the AVR32 UC3-A3 is 12MHz. The tick is used by
the FreeRTOS to measure time, and the tick frequency decides how often a tick
occurs. The default is 1000Hz, which means that the shortest delay or period
possible is 1ms. This is also the fastest tick rate we can run on this platform,
due to integer math limitations in FreeRTOS.

In the next few tasks we will be recording results from 5 different tests. The
spreadsheet (same as in exercise 5) is in the repository. It is also recommended
that you parameterize the contents of the tasks (either with the task arguments
or with `#define`’s) wherever appropriate, to avoid any copy-paste-edit errors.
Here’s one possible method:

```c
    struct responseTaskArgs {
        struct {
            uint32_t test;
            uint32_t response;
        } pin;
        // other args ...
    };

    static void responseTask(void* args){
        struct responseTaskArgs a = *(struct responseTaskArgs*)args;
        while(1){        
            if(gpio_pin_is_low(a.pin.test)){
                // ...
            }        
        }
    }

    int main(){
        ...
        xTaskCreate(responseTask, "", 1024, 
            (&(struct responseTaskArgs){{TEST_A, RESPONSE_A}, other args...}), 
            tskIDLE_PRIORITY + 1, NULL);
    }
```

#### TASK B

Create three tasks, one each for test A, B and C. The tasks should detect a test
signal from the BRTT by continuously reading the value (busy-wait). When the
signal is detected, the task should send the response signal by setting the
response pin low.

Perform a test with 1000 iterations. 

To improve the response time, we use our knowledge about the system and add a
`vTaskDelay(0)` after we have reacted to the pin. This is because we know that
one of the other tasks probably has to react to a test. So, instead of waiting
for the scheduler to switch tasks, we force it to reevaluate what task to run
once we have reacted. The 0-argument will put the calling task at the back of
list of runnable tasks (which means it will run again immediately if there are
no other runnable tasks).

Do a test with 1000 iterations. Compare the worst-case response times, and try
to explain why the distributions look different.

#### TASK C

As in exercise 5, we will give one of the tasks some additional work to do
before it responds to its signal. Again, the "work" is just a call to
`busy_wait_ms()`. Make task C wait 3ms before responding, while tasks A and B
respond immediately. As before, all tasks should busy-wait on the input pin. Use
`vTaskDelay(0)` after having responded to the test. Ideally, task C should
respond quite soon after 3ms has passed. Why doesn’t it?

Why would changing the priorities of the tasks not help us improve the
responsiveness of the C task? 

Hint: try it and see what happens.

#### TASK D

Instead of continuously checking if the test pin to goes low, we want to check
it intermittently. If the pin is not low, we can wait for the next scheduler
tick by calling `vTaskDelay(1)`, then loop around and test the pin again once
the scheduler wakes us. The argument (1) makes the scheduler reevaluate what
task to run, but the calling task is first in the queue on the next scheduler
tick. Note that this is not periodic execution – if the pin is low we perform
some amount of work (`busy_delay_ms`) that takes longer than the "period", which
means that we will overrun our target period in the case where the test pin is
low and we are responding to it. The core point is that all tasks must yield,
otherwise the highest priority task will never let the lower priority tasks run
at all.

Change your code so that it yields outside the response operation and run a test
with 1000 subtests. As before, A and B respond immediately, while C has to
perform 3ms of work first. Record the response time results, and compare to the
busy-waiting method.

What happened to the response times? Why?

#### TASK E

If possible, use priorities to make the response times for tasks A and B better.

What are the worst-case response times for A and B, and why?

#### TASK F (optional)

Use `vTaskDelayUntil()` to perform periodic polling of the test pin.
