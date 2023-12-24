#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "led.h"
#include "uart.h"
#include "timer.h"
#include "kernel.h"


#include "test_kernel_garbage.h"





//#define USART0_BAUD_RATE 16 // 16->115200 34->57600 207->9600

// #define F_CPU 16000000UL





int main(void){
    init_led();

    write_led0(1);
    
    serial_begin();

    serial_print("\nhello world my uart \n");


    // while(1){
    //     if(!uart0_rx_fifo_is_empty()){
    //         uint8_t data = uart0_rx_fifo_pop();
    //         serial_write_byte(data);
    //     }    
    // }

    initKernel();

    initTimer1();//ticks1 for kernel 1 shot 1ms (ticks1)
    initTimer0();//ticks0 for sys time 0.1s (ticks0)


    // startTask( (void *) &initState, 0, MINIMUM_PRIORITY, MINIMUM_STACK_SIZE );

    sei();


    // startTask( (&test_task), 0 , MINIMUM_PRIORITY, MINIMUM_STACK_SIZE );
    // startTask( (&test_task2), 0 , MEDIUM_PRIORITY, MINIMUM_STACK_SIZE );
    
    // startTask( (&test_task_print1), 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE );
    // startTask( (&test_task_print2), 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE );

    //print_hello

    // startTask( (&print_hello), 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE );
    // startTask( print_cnt, 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE )  ;
    startTask( print_cnt, 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE )  ;

    startTask( blink_led, 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE )  ;



    while(1){}
}
