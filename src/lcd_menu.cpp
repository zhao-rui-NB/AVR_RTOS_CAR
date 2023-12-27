#include "lcd_menu.h"

#include "kernel.h"

// memu page 
#define PAGE_MAIN 0
#define PAGE_CHOOSE_APP 1
#define PAGE_PID  2
#define PAGE_PID_SET 3

uint8_t btn_req[5] = {0,0,0,0,0};
uint8_t menu_state = PAGE_MAIN;
uint8_t app_id = 0;
char* app_name[] = {"PID line follow","show ADC value",  "PID"};

void menu_handler(void *arg){
    uint16_t sleep_time = 20;
    
    while(1){
        switch(menu_state){
            case PAGE_MAIN:
                lcd_clear();
                lcd_set_cursor(0,0);
                lcd_print("hello");
                lcd_set_cursor(1,0);
                lcd_print("INT6 enter menu");
                if(btn_req[BTN_OK]){
                    btn_req[BTN_OK] = 0;
                    menu_state = PAGE_CHOOSE_APP;
                    break;
                }
                sleep(sleep_time);
            case PAGE_CHOOSE_APP:
                lcd_clear();
                lcd_set_cursor(0,0);
                lcd_print(app_name[app_id]);
                
                lcd_set_cursor(1,0);
                lcd_print("choose app");
                if(btn_req[BTN_OK]){
                    btn_req[BTN_OK] = 0;
                    menu_state = PAGE_PID;
                    break;
                }
                sleep(sleep_time);

        }
            
            
            break;

    sleep(20);
    }
}

ISR(INT0_vect){
    btn_req[BTN_STOP] = 1;
	EIFR |= 0xFF;
}

ISR(INT1_vect){
    btn_req[BTN_LEFT] = 1;
	EIFR |= 0xFF;
}


ISR(INT5_vect){
	btn_req[BTN_RIGHT] = 1;
	EIFR |= 0xFF;
}

ISR(INT6_vect){
	btn_req[BTN_OK] = 1;
	EIFR |= 0xFF;
}

ISR(INT7_vect){
	btn_req[BTN_BACK] = 1;
	EIFR |= 0xFF;
}