#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "led.h"
#include "uart.h"
#include "timer.h"
#include "kernel.h"

#include "adc.h"

#include "motor_ctrl.h"

void test_task_print1(void *arg){
    while(1){
        // serial_print("1");
        serial_write_byte('1');
        // sleep(1);
    }
}
void test_task_print2(void *arg){
    while(1){
        // serial_print("2");
        serial_write_byte('2');
        // sleep(1);
    }
}


void test_task(void *arg){
    while(1){
        write_led0(1);
        _delay_ms(1000);
        sleep(1000);
        serial_print("time:");
        serial_print((int)sys_time);
        serial_print("\n");

        write_led0(0);
        _delay_ms(1000);
        sleep(1000);
        serial_print("time:");
        serial_print((int)sys_time);
        serial_print("\n");

    }
}

void test_task2(void *arg){
    while(1){
        for(int i=0;i<10000;i++){
            serial_print(i);
            serial_print("\n");
            sleep(10);
        }

    }
}


void print_hello(void *arg){
    while(1){
        // downSemaphore(&uart_lock);
        // serial_print("===================================================================== hello world ===================================================================== \n");
        // upSemaphore(&uart_lock);
    }
}

void print_cnt(void *arg){
    // int cnt = 0;
    uint16_t start_time = sys_time;
    while(1){
        // downSemaphore(&uart_lock);
        if (sys_time - start_time >= 1){
            serial_print("[kernel] time tick3:");
            serial_print((int)sys_time);
            serial_print("\n");
            start_time = sys_time;
        }
        // upSemaphore(&uart_lock);
        
        //sleep(10);

    }
}

void print_cnt2(void *arg){
    // int cnt = 0;
    uint16_t start_time = ticks1;
    while(1){
        // downSemaphore(&uart_lock);
        if (ticks1 - start_time >= 1000){
            serial_print("[kernel] time tick1:");
            serial_print((int)ticks1);
            serial_print("\n");
            start_time = ticks1;
        }
        // upSemaphore(&uart_lock);
        
        //sleep(10);

    }
}

void print_cnt_sleep(void *arg){
    // int cnt = 0;
    while(1){
        serial_print("[kernel] time tick1:");
        serial_print((int)ticks1);
        serial_print("\n");
        sleep(99);
    }
        // upSemaphore(&uart_lock);
        

    
}


void blink_led(void *arg){
    while(1){
        write_led0(1);
        sleep(1000);
        write_led0(0);
        sleep(1000);
    }
}


void motor_test(void *arg){
    
    
    while(1){
        motor_write_dir(MOTOR_DIR_FF);
        motor_write_speed(102,102);
        serial_print("motor FF\n");
        sleep(1000);
        while(1){}
        motor_write_dir(MOTOR_DIR_BB);
        motor_write_speed(200,200);
        serial_print("motor BB\n");
        sleep(1000);
    }
    // motor_write_dir(MOTOR_DIR_FS);
    // motor_write_speed(60000,60000);
    // _delay_ms(1000);
    // motor_write_dir(MOTOR_DIR_SF);
    // motor_write_speed(60000,60000);
    // _delay_ms(1000);
    // motor_write_dir(MOTOR_DIR_FB);
    // motor_write_speed(60000,60000);
    // _delay_ms(1000);
    // motor_write_dir(MOTOR_DIR_BF);
    // motor_write_speed(60000,60000);
    // _delay_ms(1000);
    // motor_write_dir(MOTOR_DIR_BS);
    // motor_write_speed(60000,60000);
    // _delay_ms(1000);
    // motor_write_dir(MOTOR_DIR_SB);
    // motor_write_speed(60000,60000);
    // _delay_ms(1000);
    // motor_write_dir(MOTOR_DIR_SS);
    // motor_write_speed(60000,60000);
    // _delay_ms(1000);
}

void adc_test(void *arg){
    while(1){
        // avg adc buffer 
        uint16_t L = 0;
        uint16_t M = 0;
        uint16_t R = 0;
        uint16_t BAT = 0;

        for(int i=0;i<ADC_L_BUFFER_SIZE;i++){
            L += adc_L_buffer[i];
        }
        for(int i=0;i<ADC_M_BUFFER_SIZE;i++){
            M += adc_M_buffer[i];
        }
        for(int i=0;i<ADC_R_BUFFER_SIZE;i++){
            R += adc_R_buffer[i];
        }
        for(int i=0;i<ADC_BAT_BUFFER_SIZE;i++){
            BAT += adc_BAT_buffer[i];
        }
        L /= ADC_L_BUFFER_SIZE;
        M /= ADC_M_BUFFER_SIZE;
        R /= ADC_R_BUFFER_SIZE;
        BAT /= ADC_BAT_BUFFER_SIZE;

        serial_print("L\tM\tR\tBAT\n");
        serial_print((int)L);
        serial_print("\t");
        serial_print((int)M);
        serial_print("\t");
        serial_print((int)R);
        serial_print("\t");
        serial_print((int)BAT);
        serial_print("\n");

        
        sleep(100);
    }
}