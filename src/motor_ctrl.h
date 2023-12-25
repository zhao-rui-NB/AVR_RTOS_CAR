#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


// motorL: PB3 PB2      
// F        1   0   
// B        0   1
// S        0   0

//  motorR: PB1 PB0
// F        1   0
// B        0   1
// S        0   0

// dir F:forward B:backward S:stop
//
// motor          LR
#define MOTOR_DIR_FF    0b1010        
#define MOTOR_DIR_FB    0b1001
#define MOTOR_DIR_FS    0b1000
#define MOTOR_DIR_BF    0b0110
#define MOTOR_DIR_BB    0b0101
#define MOTOR_DIR_BS    0b0100
#define MOTOR_DIR_SF    0b0010
#define MOTOR_DIR_SB    0b0001
#define MOTOR_DIR_SS    0b0000

void motor_timer_init();

void motor_write_dir(uint8_t dir);
void motor_write_speed(uint16_t speedL, uint16_t speedR);
void motor_write_speed_dir(uint8_t dir, uint8_t speedL, uint8_t speedR);














#endif //__MOTOR_CONTROL_H__
