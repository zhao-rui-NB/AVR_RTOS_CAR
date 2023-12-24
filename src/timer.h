#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

extern volatile uint16_t ticks1;  // 1ms
extern volatile uint16_t sys_time;  // 0.1s

void initTimer1(); //ticks1 for kernel 1 shot 1ms (ticks1)

#endif
