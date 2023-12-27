#ifndef __LINE_FOLLOW_H__
#define __LINE_FOLLOW_H__

#include <avr/io.h>


// void task_pid_line_follow();


#define TARGRT_SPEED 75 //75ok

// test v3 value 
#define TEST_ADC_L_BLACK 43
#define TEST_ADC_M_BLACK 61
#define TEST_ADC_R_BLACK 35
#define TEST_ADC_L_WHITE 488
#define TEST_ADC_M_WHITE 379
#define TEST_ADC_R_WHITE 487

extern uint8_t stop_line_follow;


void task_pid_line_follow_V3(void *arg);




#endif // __LINE_FOLLOW_H__