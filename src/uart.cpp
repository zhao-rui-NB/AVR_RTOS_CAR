#include "uart.h"
#include "kernel.h"
#include <stdlib.h>

Semaphore uart_lock;

void serial_begin(){
    // double usart transmission speed
	UCSR0A = (1 << U2X0);

	// rx complete interrupt , enable receiver and transmitter
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

	// 8N1	// asynchronous	// parity disabled	// 1 stop bit	// character size 8 bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	// baud rate
	UBRR0H = (uint8_t)(UART0_BAUD_RATE>>8);
	UBRR0L = (uint8_t) UART0_BAUD_RATE;

    initSemaphore(&uart_lock, 1);
}

void serial_write_byte(uint8_t data){
    while ( !( UCSR0A & (1 << UDRE0) ) ){}
    UDR0 = data;
}

void serial_write(uint8_t data){
    cli();
    serial_write_byte(data);
    sei();
}

void serial_write_buffer(uint8_t *buffer, uint8_t len){
    cli();
    for(uint8_t i = 0; i < len; i++){
        serial_write_byte(buffer[i]);
    }
    sei();
}

void serial_print(uint8_t* data){
    cli();
    while(*data != '\0'){
        serial_write_byte(*data);
        data++;
    }
    sei();
}

void serial_print(const char* data){
    cli();
    while(*data != '\0'){
        serial_write_byte((uint8_t)(*data));
        data++;
    }
    sei();
}

void serial_print(int data){
    // print a number if < 0 put - before the number
    cli();
    uint8_t buffer[15];
    for(uint8_t i = 0; i < 15; i++){
        buffer[i] = 0;
    }
    itoa(data, (char *)buffer, 10);

    serial_print(buffer);
    sei();
}
void serial_print(uint8_t data){
    serial_print((int)data);
}


volatile uint8_t uart0_rx_fifo_head = 0;
volatile uint8_t uart0_rx_fifo_data_cnt = 0;
volatile uint8_t uart0_rx_fifo[UART0_RX_FIFO_SIZE];

// RX fifo 

void uart0_rx_fifo_push(uint8_t data){
    if (uart0_rx_fifo_data_cnt < UART0_RX_FIFO_SIZE){
        uart0_rx_fifo[(uart0_rx_fifo_head+uart0_rx_fifo_data_cnt)%UART0_RX_FIFO_SIZE] = data;
        uart0_rx_fifo_data_cnt++;
    }
}

uint8_t uart0_rx_fifo_pop(){
    if(uart0_rx_fifo_data_cnt > 0){
        uint8_t data = uart0_rx_fifo[uart0_rx_fifo_head];
        uart0_rx_fifo_head = (uart0_rx_fifo_head+1)%UART0_RX_FIFO_SIZE;
        uart0_rx_fifo_data_cnt--;
        return data;
    }
    return -1;
}

// if full
uint8_t uart0_rx_fifo_is_full(){
    return uart0_rx_fifo_data_cnt == UART0_RX_FIFO_SIZE;
}
// if empty
uint8_t uart0_rx_fifo_is_empty(){
    return uart0_rx_fifo_data_cnt == 0;
}

// ISR rx 

ISR(USART0_RX_vect){
    cli();
    uart0_rx_fifo_push(UDR0);
    sei();
}
    
