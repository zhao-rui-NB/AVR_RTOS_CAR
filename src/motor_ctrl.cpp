#include "motor_ctrl.h"


void motor_timer_init(){ // timer3
    // 10 bit fast pwm TOP= 0x03FF , 0-1023
    TCCR3A=(1<<COM3A1)|(1<<COM3B1)|(1<<WGM31)|(1<<WGM30);
    TCCR3B=(1<<WGM32)|(1<<CS32); // clk/256
    // TCCR3C=(1<<FOC3A)|(1<<FOC3B); //
    OCR3A = 0;	//PE3(OC3A)(MOTOR_L)
    OCR3B = 0;	//PE4(OC3B)(MOTOR_R)

	DDRB = 0x0F;
    // DDRE = 0x1F;
    DDRE |= (1<<PE3)|(1<<PE4);
    PORTB &= ~0b1111;

}

void motor_write_dir(uint8_t dir){
    PORTB = (PORTB & ~0b1111) | dir;
}

void motor_write_speed(uint16_t speedL, uint16_t speedR){
    OCR3A = speedL;
    OCR3B = speedR;
}


// void motor_write_speed_dir(uint8_t dir, uint8_t speedL, uint8_t speedR);

