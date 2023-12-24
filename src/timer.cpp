#include "timer.h"
#include "kernel.h"
#include "uart.h"


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint16_t ticks1; //ticks1 for kernel 1 shot 1ms (ticks1)
volatile uint16_t sys_time; // 0.1s




void initTimer1(){
    TCCR1A = 0;
    // 16MHz/256
    //TCCR1B = (1 << CS12);
    // 16MHz/64
    TCCR1B = (1 << CS11) | (1 << CS10);
    TIFR |= (1 << TOV1);
    // enable Timer1 overflow interrupt
    TIMSK |= (1 << TOIE1);
} // initTimer1

// void initTimer3(){
//     TCCR3A = 0;
//     // 16MHz/256
//     TCCR3B = (1 << CS32);
//     TCCR3C = 0;
//     ETIFR |= (1 << TOV3);
//     // enable Timer3 overflow interrupt
//     ETIMSK |= (1 << TOIE3);
  
// } // initTimer3






ISR(TIMER1_OVF_vect, ISR_NAKED) {
    // save state
    // general and special purpose registers
    asm volatile(  "push __zero_reg__\n\
                    clr  __zero_reg__\n\
                    push __tmp_reg__\n\
                    in __tmp_reg__,__SREG__\n\
                    push __tmp_reg__\n\
                    in __tmp_reg__,0x3b\n\
                    push __tmp_reg__\n\
                    push r2\n\
                    push r3\n\
                    push r4\n\
                    push r5\n\
                    push r6\n\
                    push r7\n\
                    push r8\n\
                    push r9\n\
                    push r10\n\
                    push r11\n\
                    push r12\n\
                    push r13\n\
                    push r14\n\
                    push r15\n\
                    push r16\n\
                    push r17\n\
                    push r18\n\
                    push r19\n\
                    push r20\n\
                    push r21\n\
                    push r22\n\
                    push r23\n\
                    push r24\n\
                    push r25\n\
                    push r26\n\
                    push r27\n\
                    push r28\n\
                    push r29\n\
                    push r30\n\
                    push r31\n"
            :
            :
    );

    //  changeLED( AUX_LED, LED_TOGGLE );

    if ( current_task != MAXIMUM_TASKS ){
        task[current_task].stackPtr = (void *) SP;
    }
    
    // zhaorui add
    static uint8_t cnt_for_100ms = 0;
    cnt_for_100ms++;
    if (cnt_for_100ms == 100) {
        cnt_for_100ms = 0;
        sys_time++;
    }

    /*// 1 shot 10ms
    // 16MHz / 256 * 1/100 = 625
    // 2^16 - 625 = 64911
    TCNT1 = 64911;*/
    
    // 1 shot 1ms
    // 16MHz / 64 * 1/1000 = 250
    // 2^16 - 250 = 65286
    TCNT1 = 65286;

    
    ticks1++;
    
    // schedule
    schedule();
    
    // restore state	
  
    #if 0
    TIMSK = TIMSK & ~(1 << TOIE1);
    sei();
    //  transmitStringUSART1( "Task\n" );
    #if 0
    if (current_task != IDLE)
        {
        transmitStringUSART1( "Wrong TID\n" );
        }
    else
        {
        transmitStringUSART1( "TID ok\n" );
        }
    #endif

    #if 0
    if (task[current_task].stackPtr != (void *)1234)
        {
        transmitStringUSART1( "Wrong SP\n" );
        }
    else
        {
        transmitStringUSART1( "SP Ok\n" );
        }
    #endif
    //  transmitStringUSART1( "Hello World\n" );
    #endif

    if ( current_task != MAXIMUM_TASKS ){
        SP = (uint16_t) task[current_task].stackPtr;
    }      

    #if 0
    transmitBufferUSART1( (void *)SP, 48 );
    #endif
    // general and special purpose registers
    asm volatile(   "\t   pop r31\n\
                    \t   pop r30\n\
                    \t   pop r29\n\
                    \t   pop r28\n\
                    \t   pop r27\n\
                    \t   pop r26\n\
                    \t   pop r25\n\
                    \t   pop r24\n\
                    \t   pop r23\n\
                    \t   pop r22\n\
                    \t   pop r21\n\
                    \t   pop r20\n\
                    \t   pop r19\n\
                    \t   pop r18\n\
                    \t   pop r17\n\
                    \t   pop r16\n\
                    \t   pop r15\n\
                    \t   pop r14\n\
                    \t   pop r13\n\
                    \t   pop r12\n\
                    \t   pop r11\n\
                    \t   pop r10\n\
                    \t   pop r9\n\
                    \t   pop r8\n\
                    \t   pop r7\n\
                    \t   pop r6\n\
                    \t   pop r5\n\
                    \t   pop r4\n\
                    \t   pop r3\n\
                    \t   pop r2\n\
                    \t   pop __tmp_reg__\n\
                    \t   out 0x3b,__tmp_reg__\n\
                    \t   pop __tmp_reg__\n\
                    \t   out __SREG__,__tmp_reg__\n\
                    \t   pop __tmp_reg__\n\
                    \t   pop __zero_reg__\n"
            :
            :
    );
    #if 0
    #if 0
    if ( current_task != MAXIMUM_TASKS )
        {
        transmitBufferUSART1( (void *)SP, 2 );
        transmitStringUSART1( "Ret Addr\n" );
        }
    else
        {
        transmitStringUSART1( "Bad TID\n" );
        }
    #endif
    TIMSK = TIMSK | (1 << TOIE1);
    #endif
    asm volatile("\t reti\n"::);
}

// ISR(TIMER3_OVF_vect){ // set 0.1s
//     // 1 shot 1/10s
//     // 16MHz / 256 * 1/10 = 6250
//     // 2^16 - 6250 = 59286
//     TCNT3 = 59286;
//     ticks3++;
// }
