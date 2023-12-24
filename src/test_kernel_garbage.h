#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "led.h"
#include "uart.h"
#include "timer.h"
#include "kernel.h"

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
        serial_print((int)ticks3);
        serial_print("\n");

        write_led0(0);
        _delay_ms(1000);
        sleep(1000);
        serial_print("time:");
        serial_print((int)ticks3);
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
    uint16_t start_time = ticks3;
    while(1){
        // downSemaphore(&uart_lock);
        if (ticks3 - start_time >= 10){
            serial_print("[kernel] time tick3:");
            serial_print((int)ticks3);
            serial_print("\n");
            start_time = ticks3;
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