#include "lcd_menu.h"

#include "kernel.h"
#include "uart.h"
#include "timer.h"    
#include "adc.h"
#include "line_follow.h"



// memu page 
#define PAGE_MAIN 0
#define PAGE_CHOOSE_APP 1

// app page
#define PAGE_PID_LINE_FOLLOW 3
#define PAGE_ADC_SHOW  4
#define PAGE_SET_TARGET_SPEED 5
#define PAGE_LEARN_COLOR_VAL 6
#define PAGE_PID_VAL_SET 7



uint8_t btn_req[5] = {0,0,0,0,0};
uint8_t menu_state = PAGE_MAIN;
uint8_t menu_state_layer2 = 0;
uint8_t total_app = 5;
uint8_t app_id = 0;

// app_name and app_id_to_page MUST have same order 
const char* app_name[] = { // 16 char max // print at lcd line 0
    "PID line follow",
    "show ADC value",  
    "set target speed",
    "learn color val",
    "PID value set",
};
uint8_t app_id_to_page[] = {
    PAGE_PID_LINE_FOLLOW, 
    PAGE_ADC_SHOW, 
    PAGE_SET_TARGET_SPEED,
    PAGE_LEARN_COLOR_VAL,
    PAGE_PID_VAL_SET
};


#define BTN_DEBUNCE_TIME 2 // 2 ms 
uint16_t last_btn_INT_time = 0;

void lcd_app_print_adc(){
        lcd_clear();
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
        
}




void lcd_menu_btn_init(){

    EIMSK = (1<<INT7)|(1<<INT6)|(1<<INT5)|(1<<INT1)|(1<<INT0);
    //ISCn1 ISCn0
    // 00 - low level
    // 01 - 
    // 10 - negedge
    // 11 - posedge


    // enable INT  0 1 4 6 7 negedge
    EICRA = (1<<ISC01) | (1<<ISC11);
    EICRB = (1<<ISC51) | (1<<ISC61) | (1<<ISC71);

    EIMSK = (1<<INT7)|(1<<INT6)|(1<<INT5)|(1<<INT1)|(1<<INT0);


    DDRD &= ~((1<<DDD1)|(1<<DDD0));
    DDRE &= ~((1<<DDE7)|(1<<DDE6)|(1<<DDE5));

    PORTD |= (1<<PORTD1)|(1<<PORTD0);
    PORTE |= (1<<PORTE7)|(1<<PORTE6)|(1<<PORTE5);

	EIFR |= 0xFF;


}

void lcd_menu_handler(void *arg){
    uint16_t sleep_time = 200;
    uint8_t pass_this_sleep = 0;
    uint8_t last_btn_press_time = 0;
    uint8_t last_menu_state = 0;
    uint8_t have_new_frame = 1;
    uint8_t need_start_task = 0;
    uint16_t start_time = 0;
    // uint8_t BTN_DEBUNCE_TIME = 30;
    while(1){
        pass_this_sleep = 0;
        if(menu_state==PAGE_MAIN){
            if(have_new_frame){
                have_new_frame = 0;
                lcd_clear();
                lcd_set_cursor(0,0);
                lcd_print("@ Xiao Po Che  @");
                lcd_set_cursor(1,0);
                lcd_print("INT6 enter menu");
            }
            
            
            // handle button
                if(btn_req[BTN_OK]){
                    btn_req[BTN_OK] = 0;
                    last_btn_press_time = ticks1;
                    menu_state = PAGE_CHOOSE_APP;
                    pass_this_sleep = 1;
                }

        }
        else if(menu_state==PAGE_CHOOSE_APP){    
            if(have_new_frame){
                have_new_frame = 0;
                lcd_clear();
                //"app name ____"
                lcd_set_cursor(0,0);
                lcd_print(app_name[app_id]);
                

                //"choose app "1/3"
                lcd_set_cursor(1,0);
                lcd_print("choose app");
                lcd_set_cursor(1,11);
                lcd_print_int(app_id+1,2);
                lcd_print("/");
                lcd_print_int(total_app,2);
            }

            if(btn_req[BTN_OK]){
                btn_req[BTN_OK] = 0;
                last_btn_press_time = ticks1;
                menu_state = app_id_to_page[app_id];
                menu_state_layer2 = 0;
                pass_this_sleep = 1;
                if(menu_state==PAGE_PID_LINE_FOLLOW){
                    need_start_task = 1;
                }
            }
            else if(btn_req[BTN_LEFT]){
                btn_req[BTN_LEFT] = 0;
                last_btn_press_time = ticks1;
                if(app_id>0)
                    app_id--;
                else
                    app_id = total_app-1;
                have_new_frame = 1;
            }
            else if(btn_req[BTN_RIGHT]){
                btn_req[BTN_RIGHT] = 0;
                last_btn_press_time = ticks1;
                if(app_id<total_app-1)
                    app_id++;
                else
                    app_id = 0;
                have_new_frame = 1;
            }
        }

        else if(menu_state==PAGE_ADC_SHOW){
            lcd_app_print_adc();
            if(btn_req[BTN_BACK] || btn_req[BTN_OK]){
                btn_req[BTN_BACK] = 0;
                btn_req[BTN_OK] = 0;
                menu_state = PAGE_CHOOSE_APP;
                pass_this_sleep = 1;
            }
        }
        
        else if(menu_state==PAGE_SET_TARGET_SPEED){
            if(have_new_frame){
                have_new_frame = 0;
                lcd_clear();
                //"app name ____"
                lcd_set_cursor(0,0);
                lcd_print(app_name[app_id]);
                
                //"speed    75/1023"
                lcd_set_cursor(1,0);
                lcd_print("speed");
                lcd_set_cursor(1,6);
                lcd_print_int(lf_target_speed,4);
                lcd_print("/");
                lcd_print_int(1023,4);
            }
            if(btn_req[BTN_OK]){
                btn_req[BTN_OK] = 0;
                last_btn_press_time = ticks1;
                menu_state = PAGE_CHOOSE_APP;
                pass_this_sleep = 1;
            }
            else if(btn_req[BTN_LEFT]){
                btn_req[BTN_LEFT] = 0;
                last_btn_press_time = ticks1;
                if(lf_target_speed>0)
                    lf_target_speed--;
                have_new_frame = 1;
                
            }
            else if(btn_req[BTN_RIGHT]){
                btn_req[BTN_RIGHT] = 0;
                last_btn_press_time = ticks1;
                if(lf_target_speed<1023)
                    lf_target_speed++;
                have_new_frame = 1;
            }

            // read btn from io , if press that is long press
            else if(read_btn_left() &&  ticks1 - last_btn_press_time > 1000){
                // last_btn_press_time = ticks1;
                if(lf_target_speed>0)
                    lf_target_speed-=10;
                have_new_frame = 1;
            }
            else if(read_btn_right() && ticks1 - last_btn_press_time > 1000){
                // last_btn_press_time = ticks1;
                if(lf_target_speed<1023)
                    lf_target_speed+=10;
                have_new_frame = 1;
            }
        }

        else if(menu_state==PAGE_PID_VAL_SET){
            if(have_new_frame){
                have_new_frame = 0;
                //0: view kp
                //1: view ki
                //2: view kd
                //10: set kp
                //11: set ki
                //12: set kd
                lcd_clear();
                lcd_set_cursor(0,0);
                if(menu_state_layer2/10 == 0){
                    lcd_print(app_name[app_id]);
                }
                else if(menu_state_layer2/10 == 1){
                    lcd_print("INT4 INT7 modify");
                }
                else{
                    lcd_print("error");
                }
                
                lcd_set_cursor(1,0);
                if(menu_state_layer2%10 == 0){
                    lcd_print("Kp");
                    lcd_set_cursor(1,3);
                    lcd_print_int(lf_kp,4);
                }
                else if(menu_state_layer2%10 == 1){
                    lcd_print("Ki");
                    lcd_set_cursor(1,3);
                    lcd_print_int(lf_ki,4);
                }
                else if(menu_state_layer2%10 == 2){
                    lcd_print("Kd");
                    lcd_set_cursor(1,3);
                    lcd_print_int(lf_kd,4);
                }
                else{
                    lcd_set_cursor(1,0);
                    lcd_print("error");
                }

                lcd_set_cursor(1,10);
                lcd_print("bkINT1");
                
            }//if(have_new_frame)
            if(btn_req[BTN_LEFT]){
                btn_req[BTN_LEFT] = 0;
                last_btn_press_time = ticks1;
                have_new_frame = 1;
                if(menu_state_layer2/10 == 0){
                    if(menu_state_layer2>0)//0-2 kp ki kd
                        menu_state_layer2--;
                    else
                        menu_state_layer2 = 2;
                    
                }
                else if(menu_state_layer2/10 == 1){
                    if(menu_state_layer2==10)//adj kp
                        lf_kp--;
                    else if(menu_state_layer2==11)//adj ki
                        lf_ki--;
                    else if(menu_state_layer2==12)//adj kd
                        lf_kd--;
                }
            }
            else if(btn_req[BTN_RIGHT]){
                btn_req[BTN_RIGHT] = 0;
                last_btn_press_time = ticks1;
                have_new_frame = 1;
                if(menu_state_layer2/10 == 0){
                    if(menu_state_layer2<2)//0-2 kp ki kd
                        menu_state_layer2++;
                    else
                        menu_state_layer2 = 0;
                    
                }
                else if(menu_state_layer2/10 == 1){
                    if(menu_state_layer2==10)//adj kp
                        lf_kp++;
                    else if(menu_state_layer2==11)//adj ki
                        lf_ki++;
                    else if(menu_state_layer2==12)//adj kd
                        lf_kd++;   
                }
            }
            else if(btn_req[BTN_OK]){
                btn_req[BTN_OK] = 0;
                last_btn_press_time = ticks1;
                have_new_frame = 1;
                if(menu_state_layer2/10 == 0){
                    menu_state_layer2 = 10 + menu_state_layer2%10;
                }
                else if(menu_state_layer2/10 == 1){
                    menu_state_layer2 = 0 + menu_state_layer2%10;
                }

            }
            else if(btn_req[BTN_BACK]){
                btn_req[BTN_BACK] = 0;
                last_btn_press_time = ticks1;
                have_new_frame = 1;
                menu_state = PAGE_CHOOSE_APP;
                pass_this_sleep = 1;
                
                menu_state_layer2 = 0;
                
            }

            // read btn from io , if press that is long press
            else if(read_btn_left() &&  ticks1 - last_btn_press_time > 1000){
                if(lf_kp>0)
                    lf_kp-=10;
                have_new_frame = 1;
            }
            else if(read_btn_right() && ticks1 - last_btn_press_time > 1000){
                if(lf_kp<1023)
                    lf_kp+=10;
                have_new_frame = 1;
            }
        }

        else if(menu_state==PAGE_LEARN_COLOR_VAL){
            //
            // if(have_new_frame){
            //     have_new_frame = 0;
            //     lcd_clear();
            //     lcd_set_cursor(0,0);
            //     lcd_print("learn color val");
            // }
            
            // print adc value and print "put on black"


            // menu_state_layer2
            // 0: learn black
            // 1: learn white
            // 2: show result

            if(menu_state_layer2==0){// learn black
                lcd_clear();
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
                lcd_print("at black thenOK");

                if(btn_req[BTN_OK]){
                    btn_req[BTN_OK] = 0;
                    uint8_t w_size = 5;

                    lf_adc_L_black = 0;
                    lf_adc_M_black = 0;
                    lf_adc_R_black = 0;
                    
                    for(uint8_t i=0 ; i<w_size ; i++){
                        sleep(300);
                        lf_adc_L_black += adc_L_get_avg();
                        lf_adc_M_black += adc_M_get_avg();
                        lf_adc_R_black += adc_R_get_avg();
                    }
                    lf_adc_L_black /= w_size;
                    lf_adc_M_black /= w_size;
                    lf_adc_R_black /= w_size;

                    menu_state_layer2 = 1;
                }

                
            }

            else if(menu_state_layer2==1){ //learn white
                lcd_clear();
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
                lcd_print("at white thenOK");


                if(btn_req[BTN_OK]){
                    btn_req[BTN_OK] = 0;
                    uint8_t w_size = 5;

                    lf_adc_L_white = 0;
                    lf_adc_M_white = 0;
                    lf_adc_R_white = 0;
                    
                    for(uint8_t i=0 ; i<w_size ; i++){
                        sleep(300);
                        lf_adc_L_white += adc_L_get_avg();
                        lf_adc_M_white += adc_M_get_avg();
                        lf_adc_R_white += adc_R_get_avg();
                    }
                    lf_adc_L_white /= w_size;
                    lf_adc_M_white /= w_size;
                    lf_adc_R_white /= w_size;

                    menu_state_layer2 = 2;
                    start_time = ticks1;
                    have_new_frame = 1;
                }
            }

            

            else if(menu_state_layer2==2){ //show "  learn resaut  "
                //"bL 43 M 61 R 35"
                //"wL488 M379 R487"
                if(have_new_frame){
                    have_new_frame = 0;
                    lcd_clear();
                    lcd_set_cursor(0,0);
                    lcd_print("  learn resaut  ");
                    start_time = ticks1;
                }

                if(ticks1-start_time > 1500){
                    // dis "  learn resaut  " 3000ms
                    have_new_frame = 1;
                    menu_state_layer2 = 3;
                }
            }
            else if(menu_state_layer2==3){ //show res
                if(have_new_frame){
                    have_new_frame = 0;
                    
                    lcd_clear();
                    lcd_set_cursor(0,0);
                    lcd_print("bL");
                    lcd_print_int(lf_adc_L_black , 3);
                    lcd_print(" M");
                    lcd_print_int(lf_adc_M_black , 3);
                    lcd_print(" R");
                    lcd_print_int(lf_adc_R_black , 3);

                    lcd_set_cursor(1,0);
                    lcd_print("wL");
                    lcd_print_int(lf_adc_L_white , 3);
                    lcd_print(" M");
                    lcd_print_int(lf_adc_M_white , 3);
                    lcd_print(" R");
                    lcd_print_int(lf_adc_R_white , 3);
                }        
                    
                if(btn_req[BTN_OK] || btn_req[BTN_BACK]){
                    btn_req[BTN_OK] = 0;
                    btn_req[BTN_BACK] = 0;
                    menu_state = PAGE_CHOOSE_APP;
                    menu_state_layer2 = 0;
                    pass_this_sleep = 1;
                }
                


            }




        }
        
        else if(menu_state==PAGE_PID_LINE_FOLLOW){
            if(have_new_frame){
                have_new_frame = 0;
                lcd_clear();
                lcd_set_cursor(0,0);
                lcd_print("PID line follow");
                lcd_set_cursor(1,0);
                lcd_print("push back stop");
            }
            if(need_start_task){
                need_start_task = 0;
                stop_line_follow = 0;
                startTask(task_pid_line_follow_V3, 0 , MAXIMUM_PRIORITY , MAXIMUM_STACK_SIZE);
            }
            if(btn_req[BTN_BACK] || btn_req[BTN_OK]){
                stop_line_follow = 1; // send to line follow task
                btn_req[BTN_BACK] = 0;
                btn_req[BTN_OK] = 0;
                menu_state = PAGE_MAIN;
                pass_this_sleep = 1;
            }
            
        }
        
        else{
            lcd_clear();
            lcd_set_cursor(0,0);
            lcd_print("page not exist");
        
            lcd_set_cursor(1,0);
            lcd_print("push ok to back");
            if(btn_req[BTN_OK]){
                btn_req[BTN_OK] = 0;
                menu_state = PAGE_MAIN;
                pass_this_sleep = 1;
            }
        }


        if(last_menu_state != menu_state){
            have_new_frame = 1;
        }
        last_menu_state = menu_state;
        // sleep if no button pressed
        if(!pass_this_sleep){
            sleep(sleep_time);
        }
    }
}




ISR(INT0_vect){
    if(sys_time-last_btn_INT_time > BTN_DEBUNCE_TIME){
        btn_req[BTN_STOP] = 1;
        last_btn_INT_time = sys_time;
        // serial_print("INT0\n");
    }
	EIFR |= 0xFF;
}

ISR(INT1_vect){
    if(sys_time-last_btn_INT_time > BTN_DEBUNCE_TIME){
        btn_req[BTN_BACK] = 1;
        last_btn_INT_time = sys_time;
        // serial_print("INT1\n");
    }
	EIFR |= 0xFF;
}


ISR(INT5_vect){
    if(sys_time-last_btn_INT_time > BTN_DEBUNCE_TIME){
        btn_req[BTN_LEFT] = 1;
        last_btn_INT_time = sys_time;
        // serial_print("INT5\n");
    }
	EIFR |= 0xFF;
}

ISR(INT6_vect){
	if(sys_time-last_btn_INT_time > BTN_DEBUNCE_TIME){
        btn_req[BTN_OK] = 1;
        last_btn_INT_time = sys_time;
        // serial_print("INT6\n");
    }
	EIFR |= 0xFF;
}

ISR(INT7_vect){
    if(sys_time-last_btn_INT_time > BTN_DEBUNCE_TIME){
        btn_req[BTN_RIGHT] = 1;
        last_btn_INT_time = sys_time;
        // serial_print("INT7\n");
    }
	EIFR |= 0xFF;
}