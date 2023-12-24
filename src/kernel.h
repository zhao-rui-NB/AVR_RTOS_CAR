#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <stdint.h>

#define MAXIMUM_TASKS 8

enum task_id {IDLE, TID1, TID2, TID3, TID4, TID5, TID6, TID7};
enum task_state {RUNNING, READY, BLOCKED, STOPPED};
enum task_prioriy {IDLE_PRIORITY, MINIMUM_PRIORITY, MEDIUM_PRIORITY, MAXIMUM_PRIORITY};

struct tcb { //Task Control Block 
	uint8_t tid;
	uint8_t state;
	uint8_t priority;
	int8_t niceness;
	void *stackPtr;
	uint16_t sleep;
};
typedef struct tcb TCB;

struct semaphore {
	int8_t value;
	uint8_t wait_list[MAXIMUM_TASKS];
};
typedef struct semaphore Semaphore;

// 4 PC (stop function, start function) + 32 GPR + 1 SREG + 1 RAMPZ = 38
#define IDLE_STACK_SIZE 64
#define MINIMUM_STACK_SIZE 128
#define MEDIUM_STACK_SIZE 256
#define MAXIMUM_STACK_SIZE 512
#define RAIL_STACK_SIZE 9
#define STACK_SIZE (IDLE_STACK_SIZE) + (MINIMUM_STACK_SIZE*4) + (MEDIUM_STACK_SIZE*2) + (MAXIMUM_STACK_SIZE) + (RAIL_STACK_SIZE)

#define RAIL0 STACK_SIZE - 1
#define RAIL1 STACK_SIZE - 2 - IDLE_STACK_SIZE
#define RAIL2 STACK_SIZE - 2 - IDLE_STACK_SIZE - 1 - MINIMUM_STACK_SIZE
#define RAIL3 STACK_SIZE - 2 - IDLE_STACK_SIZE - 1 - MINIMUM_STACK_SIZE*2 - 1
#define RAIL4 STACK_SIZE - 2 - IDLE_STACK_SIZE - 1 - MINIMUM_STACK_SIZE*3 - 2
#define RAIL5 STACK_SIZE - 2 - IDLE_STACK_SIZE - 1 - MINIMUM_STACK_SIZE*4 - 3
#define RAIL6 STACK_SIZE - 2 - IDLE_STACK_SIZE - 1 - MINIMUM_STACK_SIZE*4 - 4 - MEDIUM_STACK_SIZE
#define RAIL7 STACK_SIZE - 2 - IDLE_STACK_SIZE - 1 - MINIMUM_STACK_SIZE*4 - 4 - MEDIUM_STACK_SIZE*2 - 1
#define RAIL8 0
#define SCHECK 0xff

enum partition_status {FREE = 1, USED = 0};

struct partition { // stack partition ptr and size 
	void *stackPtr;
	uint16_t stackSize;
	uint8_t status;
};

typedef struct partition Partition;

extern volatile TCB task[];
extern volatile uint8_t stack[];
extern volatile uint8_t current_task;
extern volatile uint8_t total_tasks;
extern volatile uint8_t tid_free;

void initKernel(void);
uint8_t startTask(void (*functionPtr)(void *arg), void *arg, uint8_t priority, uint16_t stackSize);
void stopTask(void);
void sleep(uint16_t ticks);

void initSemaphore(Semaphore *semaphore, uint8_t value);
void downSemaphore(Semaphore *semaphore);
void upSemaphore(Semaphore *semaphore);
uint8_t tryDownSemaphore(Semaphore *semaphore);

void sinit(void);
void *salloc(uint16_t stackSize);
uint8_t scheck(void);
void sfree(void *stackPtr);
uint8_t talloc(void);
void tfree(uint8_t tid);

void idle(void *arg);
void schedule(void);

#ifdef KERNEL_DEBUG
void debug(void *arg);
void testTask0(void *arg);
void testTask1(void *arg);
void testTask2(void *arg);
#endif

#endif
