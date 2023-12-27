#ifndef __LCD_MENU_H__
#define __LCD_MENU_H__

#include "LCD.h"

#include <avr/io.h>
#include <avr/interrupt.h>



void menu_handler(void *arg);

extern uint8_t btn_req[];

#define BTN_STOP   0
#define BTN_LEFT   1 
#define BTN_RIGHT  2
#define BTN_OK     3 
#define BTN_BACK   4






#endif // __LCD_MENU_H__