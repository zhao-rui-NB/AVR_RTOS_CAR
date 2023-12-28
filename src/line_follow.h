#ifndef __LINE_FOLLOW_H__
#define __LINE_FOLLOW_H__

#include <avr/io.h>

// test v3 value 
#define TEST_ADC_L_BLACK 43
#define TEST_ADC_M_BLACK 61
#define TEST_ADC_R_BLACK 35
#define TEST_ADC_L_WHITE 488
#define TEST_ADC_M_WHITE 379
#define TEST_ADC_R_WHITE 487

// #define LINE_FOLLOW_INIT_TARGRT_SPEED 75 //75ok
#define LINE_FOLLOW_INIT_TARGRT_SPEED 85 //75ok

#define LINE_FOLLOW_INTERVAL 3


// #define LINE_FOLLOW_INIT_KP 57
// #define LINE_FOLLOW_INIT_KD 5
// #define LINE_FOLLOW_INIT_KI 0

#define LINE_FOLLOW_INIT_KP 70
#define LINE_FOLLOW_INIT_KD 8
#define LINE_FOLLOW_INIT_KI 0

#define LINE_FOLLOW_ADC_L_BLACK 43
#define LINE_FOLLOW_ADC_M_BLACK 61
#define LINE_FOLLOW_ADC_R_BLACK 35
#define LINE_FOLLOW_ADC_L_WHITE 488
#define LINE_FOLLOW_ADC_M_WHITE 379
#define LINE_FOLLOW_ADC_R_WHITE 487


extern uint8_t stop_line_follow;

extern float lf_kp;
extern float lf_kd;
extern float lf_ki;
extern int lf_target_speed;

extern int lf_adc_L_black;
extern int lf_adc_M_black;
extern int lf_adc_R_black;
extern int lf_adc_L_white;
extern int lf_adc_M_white;
extern int lf_adc_R_white;





void task_pid_line_follow_V3(void *arg);




#endif // __LINE_FOLLOW_H__