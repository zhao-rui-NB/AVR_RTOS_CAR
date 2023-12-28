#include "kernel.h"
// #include "serial.h"
#include "timer.h"

// #include "led.h"

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// bss
volatile TCB task[MAXIMUM_TASKS];
volatile uint8_t stack[STACK_SIZE];
volatile uint8_t current_task;
volatile uint8_t total_tasks;
volatile uint8_t tid_free;

// Partition 	void *stackPtr; uint16_t stackSize; uint8_t status;
Partition partition_list[] = {	
    { (void *) &stack[RAIL0 - 1], IDLE_STACK_SIZE, FREE },
    { (void *) &stack[RAIL1 - 1], MINIMUM_STACK_SIZE, FREE },
    { (void *) &stack[RAIL2 - 1], MINIMUM_STACK_SIZE, FREE },
    { (void *) &stack[RAIL3 - 1], MINIMUM_STACK_SIZE, FREE },
    { (void *) &stack[RAIL4 - 1], MINIMUM_STACK_SIZE, FREE },
    { (void *) &stack[RAIL5 - 1], MEDIUM_STACK_SIZE, FREE },
    { (void *) &stack[RAIL6 - 1], MEDIUM_STACK_SIZE, FREE },
    { (void *) &stack[RAIL7 - 1], MAXIMUM_STACK_SIZE, FREE }
};

void initKernel(void) {
    sinit();
    startTask(  &idle, 0, IDLE_PRIORITY, IDLE_STACK_SIZE );

#ifdef KERNEL_DEBUG
    startTask( (void *) &debug, 0, MINIMUM_PRIORITY, MINIMUM_STACK_SIZE );
#endif
    /* Set an invalid task for now */
    current_task = MAXIMUM_TASKS;
} // initKernel

// 0 - success
uint8_t startTask(void (*functionPtr)(void *arg), void *arg, uint8_t priority, uint16_t stackSize){
    uint8_t result = 1;
    uint8_t id;
    /*uint8_t i; */

    id = talloc();

    if (id != MAXIMUM_TASKS){
        task[id].tid = id;
        task[id].state = READY;
        task[id].priority = priority;
        task[id].stackPtr;
      
        if ( ( task[id].stackPtr  = salloc(stackSize) ) != NULL ){
            // create illusion new task was just interrupted
            uint16_t stackPtr = SP;
            SP = (uint16_t) task[id].stackPtr;
        
            asm volatile(	
                "push %0\n\t"	// PC - low byte (stop function)
                "push %1\n\t"	// PC - high byte
                "push %2\n\t"	// PC - low byte (start function)
                "push %3\n\t"	// PC - high byte
                "push __zero_reg__\n\t"	// __zero_reg__
                "push __tmp_reg__\n\t"	// __tmp_reg__
                "in __tmp_reg__,__SREG__\n\t"
                "push __tmp_reg__\n\t"	// SREG
                "in __tmp_reg__,0x3b\n\t"	
                "push __tmp_reg__\n\t"	// RAMPZ
                "push r2\n\t"	
                "push r3\n\t"	
                "push r4\n\t"	
                "push r5\n\t"	
                "push r6\n\t"	
                "push r7\n\t"	
                "push r8\n\t"	
                "push r9\n\t"	
                "push r10\n\t"	
                "push r11\n\t"	
                "push r12\n\t"	
                "push r13\n\t"	
                "push r14\n\t"	
                "push r15\n\t"	
                "push r16\n\t"	
                "push r17\n\t"	
                "push r18\n\t"	
                "push r19\n\t"	
                "push r20\n\t"	
                "push r21\n\t"	
                "push r22\n\t"	
                "push r23\n\t"	
                "push %4\n\t"	// r24 start function arg - low byte (avr-gcc call convention)
                "push %5\n\t"	// r25 start function arg - high byte
                "push r26\n\t"	
                "push r27\n\t"	
                "push r28\n\t"	
                "push r29\n\t"	
                "push r30\n\t"	
                "push r31\n\t"	
                :
                : 
                "r" ( (uint16_t) &stopTask ),		// PC - low byte (stop function)
                "r" ( ( (uint16_t) &stopTask ) >> 8 ), 	// PC - high byte
                "r" ( (uint16_t) functionPtr ),		// PC - low byte (start function)
                "r" ( ( (uint16_t) functionPtr ) >> 8 ),// PC - high byte
                "r" ( (uint16_t) arg ),			// start function arg - low byte
                "r" ( ( (uint16_t) arg ) >> 8 )		// start function arg - high byte
            );

            task[id].stackPtr  = (void *) SP;
        
        #if 0	
        for( i = 0; i < 34; i++ )
            {
            ((uint8_t *) SP)[i] = i+10;
            }
        #endif		
            SP = stackPtr;
        
            result = 0;
        
        }
    }
    return result;
} // startTask

void stopTask(void){

    #ifdef KERNEL_DEBUG
    downSemaphore(&usart1_lock);
    transmitStringUSART1("stopTask\n");
    upSemaphore(&usart1_lock);
    #endif

    task[current_task].state = STOPPED;
    tfree(current_task);
    sfree(task[current_task].stackPtr);

    while(1)
        asm volatile("nop"::);

} // stopTask

void sleep(uint16_t ticks){

    task[current_task].sleep = ticks1 + ticks;
    task[current_task].state = BLOCKED;

    while (task[current_task].sleep != 0);
    asm volatile("nop"::);

} // sleep

// blocking semaphore with priority and niceness
// internal semantics of semaphore reversed for simple array indexing
void initSemaphore(Semaphore *semaphore, uint8_t value){
  
  semaphore->value = -value;

} // initSemaphore

// note: do not call downSemaphore within an interrupt context
void downSemaphore(Semaphore *semaphore){
  
    cli();
  
    if (semaphore->value >= 0) {
    // insert in order of priority and niceness
        uint8_t x;
        for(x = 0; x < semaphore->value; x++){
        
            if( task[current_task].priority > task[ semaphore->wait_list[x] ].priority && task[current_task].niceness < task[ semaphore->wait_list[x] ].niceness ){
            
                uint8_t temp_task = semaphore->wait_list[x];
                uint8_t i;
            
                for (i = x + 1; i < semaphore->value; i++) {
                    uint8_t swap = semaphore->wait_list[i];
                    semaphore->wait_list[i] = temp_task;
                    temp_task = swap;
                }
                semaphore->wait_list[i] = temp_task;
                break;
            }
        }
        semaphore->wait_list[x] = current_task;
        task[current_task].state = BLOCKED;
    }
  
    semaphore->value++;
  
    sei();
  
    while (task[current_task].state == BLOCKED){
        asm volatile("nop"::);
    }

} // downSemaphore

void upSemaphore(Semaphore *semaphore){
    cli();
    semaphore->value--;
    if(semaphore->value >= 0){
        uint8_t x;
        task[semaphore->wait_list[0]].state = READY;
    
        for (x = 1; x <= semaphore->value; x++) {
            semaphore->wait_list[x - 1] = semaphore->wait_list[x];
        }
    
    }
  
    sei();
  
} // upSemaphore

// return value
// 0 - fail
// 1 - success
uint8_t tryDownSemaphore(Semaphore *semaphore){
    uint8_t result = 0;
    if (semaphore->value < 0){
        result = 1;
    }
    return result;
} // tryDownSemaphore

void sinit(void){ 
    stack[RAIL0] = SCHECK;
    stack[RAIL1] = SCHECK;
    stack[RAIL2] = SCHECK;
    stack[RAIL3] = SCHECK;
    stack[RAIL4] = SCHECK;
    stack[RAIL5] = SCHECK;
    stack[RAIL6] = SCHECK;
    stack[RAIL7] = SCHECK;
    stack[RAIL8] = SCHECK;
} // sinit

// first fit allocation policy
// NULL - error
void *salloc(uint16_t stackSize){
    void *result = NULL;
    uint8_t x;
    for (x = 0; x < MAXIMUM_TASKS; x++) {
    
        if (stackSize <= partition_list[x].stackSize && partition_list[x].status) {
            partition_list[x].status = USED;
            result = partition_list[x].stackPtr;
            break;
        }
    
    }  
    return result;  
} // salloc

// 1 byte rails between stack partitions containing sentinel value
// if any rail value is not equal to the sentinel value, stack overflow or underflow has occured
// 1 = success, 0 = fail
uint8_t scheck(void){
  
    uint8_t result = 1;
  
    if (	stack[RAIL0] != SCHECK ||
	stack[RAIL1] != SCHECK ||
	stack[RAIL2] != SCHECK ||
	stack[RAIL3] != SCHECK ||
	stack[RAIL4] != SCHECK ||
	stack[RAIL5] != SCHECK ||
	stack[RAIL6] != SCHECK ||
	stack[RAIL7] != SCHECK ||
	stack[RAIL8] != SCHECK )
    {
      result = 0;
    }
  
    return result;
} // scheck

void sfree(void *stackPtr){
    uint8_t x;
    for (x = 0; x < MAXIMUM_TASKS; x++){
        if ( stackPtr <= partition_list[x].stackPtr && stackPtr >= ((uint8_t*)partition_list[x].stackPtr - partition_list[x].stackSize) ){
	        partition_list[x].status = FREE;
        }
    }
} // sfree

uint8_t talloc(void){
  
    uint8_t result = MAXIMUM_TASKS;
  
    if (total_tasks < MAXIMUM_TASKS){
        for (uint8_t x = 0; x < 8; x++){
            if ( ( tid_free & (1 << x) ) == 0 ){
                tid_free |= (1 << x);
                total_tasks++;
                result = x;
                break;
            }	
        }
    }
    return result;
  
} // talloc

void tfree(uint8_t tid){
    tid_free &=  ( ~ (1 << tid) );
    total_tasks--;  
} // tfree

void idle(void *arg){
  /*  uint8_t i; */

    #ifdef KERNEL_DEBUG
    while(1) {

        downSemaphore(&usart1_lock);
        transmitStringUSART1("idle\n");
        upSemaphore(&usart1_lock);

    }
    #else

    #   if 0
    while( 1 ){
        changeLED( MANAGE_LED, LED_TOGGLE );
        for( i = 0; i < 100; i++ ){
	    _delay_ms(100);
	}      
    }
    #   else
    while(1){
        asm volatile("nop"::);
    }
    #   endif
    #endif
} // idle

// priority scheduling with niceness value to prevent starvation
void schedule(void){

    if( ( current_task < MAXIMUM_TASKS) && (task[current_task].state == RUNNING) ){
        task[current_task].state = READY;
    }
    else{
        current_task = IDLE;
    }
  
    for(uint8_t x = 0; x < MAXIMUM_TASKS; x++){
    
        if ( /*x != current_task &&*/ ( tid_free & (1 << x) ) != 0 ){
            if (task[x].sleep == ticks1) {
                task[x].sleep = 0;
                task[x].state = READY;
            }
      
            if(task[x].state == READY){
	
                // if (task[x].priority > task[current_task].priority){
                if (task[x].priority > task[current_task].priority){// rui modified
                    current_task = x;
                }
                // else if (task[x].priority <= task[current_task].priority){
                else{ // rui modified
                    if(task[x].niceness < task[current_task].niceness){
                        task[current_task].niceness--;
                        current_task = x;
                    }
                    else{
                        task[x].niceness--;
                    }
                }
            }
        }
    }
    task[current_task].niceness++;
    task[current_task].niceness++;
    task[current_task].state = RUNNING;
} // schedule

#ifdef KERNEL_DEBUG

void debug(void *arg) {
    char *test0 = "testTask0\n";
    char *test1 = "testTask1\n";
    char *test2 = "testTask2\n";
    startTask( (void *) &testTask0, (void *) test0, MINIMUM_PRIORITY, MINIMUM_STACK_SIZE );
    startTask( (void *) &testTask1, (void *) test1, MEDIUM_PRIORITY, MINIMUM_STACK_SIZE );
    startTask( (void *) &testTask2, (void *) test2, MAXIMUM_PRIORITY, MINIMUM_STACK_SIZE );

    while(1) {

        downSemaphore(&usart1_lock);
        transmitStringUSART1("debug\n");
        if ( !scheck() )
            transmitStringUSART1("scheck failed.\n");
        upSemaphore(&usart1_lock);

        sleep(1000);

    }

} // debug

void testTask0(void *arg) {
  while(1) {
    downSemaphore(&usart1_lock);
    transmitStringUSART1( (char *) arg );
    upSemaphore(&usart1_lock);
    sleep(1000);
  }

} // testTask0

void testTask1(void *arg) {
    while(1) {
        downSemaphore(&usart1_lock);
        transmitStringUSART1( (char *) arg );
        upSemaphore(&usart1_lock);

        sleep(2000);

    }

} // testTask1

void testTask2(void *arg) {

    while(1) {

        downSemaphore(&usart1_lock);
        transmitStringUSART1( (char *) arg );
        upSemaphore(&usart1_lock);

        sleep(3000);
    }

} // testTask2

#endif
