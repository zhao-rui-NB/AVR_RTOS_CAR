#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "led.h"
#include "uart.h"
#include "timer.h"
#include "kernel.h"

#include "motor_ctrl.h"

#include "adc.h"
#include "LCD.h"    

#include "test_kernel_garbage.h"

#include "line_follow.h"


// kernel test task
void task_lcd_print_adc(void *arg){
    while(1){
        lcd_set_cursor(0,0);
        lcd_print("L");
        lcd_print_int(adc_L_get_avg() , 3);

        lcd_set_cursor(0,5);
        lcd_print("M");
        lcd_print_int(adc_M_get_avg() , 3);

        lcd_set_cursor(0,10);
        lcd_print("R");
        lcd_print_int(adc_R_get_avg() , 3);

        lcd_set_cursor(1,0);
        lcd_print("BATT");
        lcd_print_int(adc_BAT_to_volt(adc_BAT_get_avg()) , 3);
        
        sleep(100);
    }
}






int main(void){
    lcd_init();

    init_led();
    write_led0(1);

    motor_timer_init();

    adc_init();

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
    // initTimer3();//ticks0 for sys time 0.1s (ticks0)


    // startTask( (void *) &initState, 0, MINIMUM_PRIORITY, MINIMUM_STACK_SIZE );



    // startTask( (&test_task), 0 , MINIMUM_PRIORITY, MINIMUM_STACK_SIZE );
    // startTask( (&test_task2), 0 , MEDIUM_PRIORITY, MINIMUM_STACK_SIZE );
    
    // startTask( (&test_task_print1), 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE );
    // startTask( (&test_task_print2), 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE );

    //print_hello

    // startTask( (&print_hello), 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE );
    // startTask( print_cnt, 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE )  ;
    // startTask( print_cnt, 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE )  ;

    startTask( blink_led, 0 , MINIMUM_PRIORITY , MINIMUM_STACK_SIZE )  ;


    // test motor
    // startTask( motor_test, 0 , MEDIUM_PRIORITY , MINIMUM_STACK_SIZE )  ;

    // test adc 
    // startTask( adc_test, 0 , MEDIUM_PRIORITY , MINIMUM_STACK_SIZE )  ;

    // startTask(test_lcd, 0 , MEDIUM_PRIORITY , MINIMUM_STACK_SIZE )  ;


    // startTask( task_lcd_print_adc, 0 , MEDIUM_PRIORITY , MINIMUM_STACK_SIZE )  ;


    // start_task()

    // teat v3
    // startTask( task_pid_line_follow_V3, 0 , MAXIMUM_PRIORITY , MEDIUM_STACK_SIZE )  ;    
    startTask( task_pid_line_follow_V3, 0 , MAXIMUM_PRIORITY , MAXIMUM_STACK_SIZE )  ;    

    // motor_write_dir(MOTOR_DIR_FF);
    // startTask( task_pid_line_follow, 0 , MAXIMUM_PRIORITY , MEDIUM_STACK_SIZE )  ;

    // sensor map 
    // startTask( sensor_map, 0 , MAXIMUM_PRIORITY , MEDIUM_STACK_SIZE )  ;


    
    serial_print("kernel start...\n");
    sei();
    while(1){}
}
