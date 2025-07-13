# README


## Introduction

This workspace contains three separate PlatformIO projects that illustrate possible ways of initialization of a FreeRTOS project.

My goal was to investigate ways that a FreeRTOS project can handle a long term initialization process where until a successful point
has been reached no further action can be taken by the user's tasks. An example of this is WiFi startup, which can take 2 or 3 seconds or sometimes longer.

I wanted to try out different ways and explore the mechanism in detail to decide which one to use for my current projects.

Each example project is a completely independent project and you can open them in the PlatformIO UI and build and run them. They have a common
structure - one `initTask` and three 'user' tasks - `TaskA`, `TaskB` and `TaskC`. Each of the 'user' tasks does nothing but print that its running.

## Description

### counting_semaphores

This uses a counting semaphore that is initially set to have a max count of 3 and an initial count of 0. Each user task is blocked until
it can obtain a semaphore. An interesting point is that the first task(s) to run will be the highest priority task(s).

### sequential_start

In this case, the user tasks are not created until the initialization phase is complete. Although this seems to work OK, I decided not to
use it for reasons discussed below.

### event_group_sync

In this case, a FreeRTOS _eventGroup_ is used. See [Event Groups](https://freertos.org/Documentation/02-Kernel/04-API-references/12-Event-groups-or-flags/00-Event-groups) in the FreeRTOS docs.

An event group is controlled by a bit mask. There are a number of options as to how the event bits are handled, and you should refer to the documentation for more details. The way this code is written each user task waits for a bit to be set. A different bit is used for each task. All three tasks can be unblocked with one action:- `xEventGroupSetBits(eventHandle, EVENT_BITS);`

## Other Options

In the past I also haved use a queue to send 'init complete' messages to other tasks. I may add an example of that for completeness.

## Conclusion

I think that the event group approach is my favorite from these options. 

1. It is simple and uses the least code - one line in the 'init' task and one line in each user task.

2. It allows for a good decoupling of concerns. The event group mask can be a project global and all tasks need to do is to
add their specific event bit to the mask. The initialization task does not need to know anything about the function of the user
tasks.
