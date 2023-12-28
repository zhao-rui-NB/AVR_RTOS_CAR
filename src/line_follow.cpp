#include "line_follow.h"

#include "adc.h"
#include "motor_ctrl.h"
#include "uart.h"
#include "kernel.h"


uint8_t stop_line_follow;

// set default value here 
float lf_kp = LINE_FOLLOW_INIT_KP;
float lf_kd = LINE_FOLLOW_INIT_KD;
float lf_ki = LINE_FOLLOW_INIT_KI;
int lf_target_speed = LINE_FOLLOW_INIT_TARGRT_SPEED;

int lf_adc_L_black = LINE_FOLLOW_ADC_L_BLACK;
int lf_adc_M_black = LINE_FOLLOW_ADC_M_BLACK;
int lf_adc_R_black = LINE_FOLLOW_ADC_R_BLACK;
int lf_adc_L_white = LINE_FOLLOW_ADC_L_WHITE;
int lf_adc_M_white = LINE_FOLLOW_ADC_M_WHITE;
int lf_adc_R_white = LINE_FOLLOW_ADC_R_WHITE;



void task_pid_line_follow_V3(void *arg){
    // stop_line_follow = 0;
    while(!stop_line_follow){
        static float integral = 0;
        static float last_error = 0;
        float error;

        // get adc value AND calc error
        float L = (float)adc_L_get_avg();
        // float M = (float)adc_M_get_avg(); // mid sensor not use
        float R = (float)adc_R_get_avg();

        // map adc to 0-1 
        float L_map = float(L-lf_adc_L_black)/float(lf_adc_L_white-lf_adc_L_black);
        //float M_map // mid sensor not use
        float R_map = float(R-lf_adc_R_black)/float(lf_adc_R_white-lf_adc_R_black);

        error = (L_map-R_map);
        if(L_map>0.95 && R_map>0.95){
            if(last_error > 0){
                error = 1.0;
            }else{
                error = -1.0;
            }
        }
        last_error = error;


        // PID control out
        // https://www.eet-china.com/mp/a119142.html?fbclid=IwAR26cIluPrgb1PNj0koLSO22H0IaeI4kiDMwyXBFAb6d3fO0tYFLTYMmFaQ 
        float Kp = lf_kp;
        float Ki = lf_ki;
        float Kd = lf_kd;
        float dt = 3; 
        

        integral += error*dt;
        float derivative = (error-last_error)/dt;

        float p = error;
        float i = integral;
        float d = derivative;

        int out = Kp*p + Ki*i + Kd*d;

        int left_speed  = lf_target_speed + out;
        int right_speed = lf_target_speed - out;
        

        // if speed is negative, change dir and make it positive
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
        // limit speed to 1023
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