#ifndef __LCD_MENU_H__
#define __LCD_MENU_H__

#include "LCD.h"

#include <avr/io.h>
#include <avr/interrupt.h>



void lcd_menu_handler(void *arg);
void lcd_menu_btn_init();
extern uint8_t btn_req[];

#define BTN_STOP   0
#define BTN_BACK   1
#define BTN_LEFT   2 
#define BTN_OK     3 
#define BTN_RIGHT  4


// read all btn state 
#define read_btn_stop()   (!(PIND & (1<<PINE0)))
#define read_btn_back()   (!(PIND & (1<<PINE1)))
#define read_btn_left()   (!(PINE & (1<<PINE5)))
#define read_btn_ok()     (!(PINE & (1<<PINE6)))
#define read_btn_right()  (!(PINE & (1<<PINE7)))






#endif // __LCD_MENU_H__