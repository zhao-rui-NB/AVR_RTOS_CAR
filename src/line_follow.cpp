#include "line_follow.h"

#include "adc.h"
#include "motor_ctrl.h"
#include "uart.h"
#include "kernel.h"


uint8_t stop_line_follow;
void task_pid_line_follow_V3(void *arg){
    // stop_line_follow = 0;
    while(!stop_line_follow){
        static float integral = 0;
        static float last_error = 0;
        float error;

        float L = (float)adc_L_get_avg();
        float M = (float)adc_M_get_avg();
        float R = (float)adc_R_get_avg();

        // map adc to 0-1 
        float L_map = float(L-TEST_ADC_L_BLACK)/float(TEST_ADC_L_WHITE-TEST_ADC_L_BLACK);
        //float M_map = (M-TEST_ADC_M_BLACK)/(TEST_ADC_M_WHITE-TEST_ADC_M_BLACK);
        float R_map = float(R-TEST_ADC_R_BLACK)/float(TEST_ADC_R_WHITE-TEST_ADC_R_BLACK);
        int pos = (L_map-R_map)*1000.0;

        error = (L_map-R_map);
        if(L_map>0.95 && R_map>0.95){
            if(last_error > 0){
                error = 1.0;
            }else{
                error = -1.0;
            }
        }

        last_error = error;

        // https://www.eet-china.com/mp/a119142.html?fbclid=IwAR26cIluPrgb1PNj0koLSO22H0IaeI4kiDMwyXBFAb6d3fO0tYFLTYMmFaQ 
        float Kp = 57;
        float Kd = 5;
        float Ki = 0;
        float dt = 3;

        
        static float prev_error = 0;
        static float derivative = 0;  

        integral += error*dt;
        derivative = (error-last_error)/dt;

        float p = error;
        float i = integral;
        float d = derivative;

        int out = Kp*p + Ki*i + Kd*d;

        int left_speed = TARGRT_SPEED + out;
        int right_speed = TARGRT_SPEED - out;
        

        if(left_speed >=0 && right_speed >=0){
            motor_write_dir(MOTOR_DIR_FF);
        }else if(left_speed < 0 && right_speed >= 0){
            motor_write_dir(MOTOR_DIR_BF);
            left_speed = left_speed*-1;
        }else if(left_speed >= 0 && right_speed < 0){
            motor_write_dir(MOTOR_DIR_FB);
            right_speed = right_speed*-1;
        }else{
            motor_write_dir(MOTOR_DIR_BB);
            left_speed = left_speed*-1;
            right_speed = right_speed*-1;
        }        
        if(left_speed > 1023) left_speed = 1023;
        if(right_speed > 1023) right_speed = 1023;


        motor_write_speed(left_speed, right_speed);


        sleep(dt);
    }

    // stop motor
    motor_write_speed(0,0);
    motor_write_dir(MOTOR_DIR_SS);
    return;

}