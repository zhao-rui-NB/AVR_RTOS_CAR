#ifndef __ADC_H__
#define __ADC_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//          adc ch
#define ADC_L   1
#define ADC_M   2
#define ADC_R   3
#define ADC_BAT 0

#define ADC_CH_CNT 4

#define ADC_L_BUFFER_SIZE 8
#define ADC_M_BUFFER_SIZE 8
#define ADC_R_BUFFER_SIZE 8
#define ADC_BAT_BUFFER_SIZE 8

extern volatile uint8_t adc_L_buffer_index ;
extern volatile uint8_t adc_M_buffer_index;
extern volatile uint8_t adc_R_buffer_index;
extern volatile uint8_t adc_BAT_buffer_index;

extern volatile uint16_t adc_L_buffer[];
extern volatile uint16_t adc_M_buffer[];
extern volatile uint16_t adc_R_buffer[];
extern volatile uint16_t adc_BAT_buffer[];


extern volatile uint8_t adc_now_ch;

void adc_init();












#endif //__ADC_H__
