#ifndef __UART_H__
#define __UART_H__

#include "kernel.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define UART0_BAUD_RATE 16 // 16->115200 34->57600 207->9600

#define UART0_RX_FIFO_SIZE 128


extern volatile uint8_t uart0_rx_fifo_head;
extern volatile uint8_t uart0_rx_fifo_data_cnt;
extern volatile uint8_t uart0_rx_fifo[UART0_RX_FIFO_SIZE];
extern Semaphore uart_lock;


// uint8_t uart0_auto_stop_isr = 1;
void serial_write_byte(uint8_t data);

void serial_begin();//default 115200
void serial_write(uint8_t data);
void serial_write_buffer(uint8_t *buffer, uint8_t len);

void serial_print(uint8_t* data);
void serial_print(const char* data);
void serial_print(int data);
void serial_print(uint8_t data);

void uart0_rx_fifo_push(uint8_t data);
uint8_t uart0_rx_fifo_pop();
uint8_t uart0_rx_fifo_is_full();
uint8_t uart0_rx_fifo_is_empty();

#endif // __UART_H__
