#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


void adc_init(){
	ADMUX=(1<<REFS0); //AREF is AVCC, (ADLAR=0) ADCH:XXXXXX98 ADCL:76543210
	ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADFR)|(1<<ADIE|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)); //DIV128		
}

volatile uint8_t adc_now_ch = 0;
volatile uint8_t adc_L_buffer_index ;
volatile uint8_t adc_M_buffer_index;
volatile uint8_t adc_R_buffer_index = 0;
volatile uint8_t adc_BAT_buffer_index = 0;
volatile uint16_t adc_L_buffer[ADC_L_BUFFER_SIZE];
volatile uint16_t adc_M_buffer[ADC_M_BUFFER_SIZE];
volatile uint16_t adc_R_buffer[ADC_R_BUFFER_SIZE];
volatile uint16_t adc_BAT_buffer[ADC_BAT_BUFFER_SIZE];
#define ADC_DROP_DATA 3


ISR(ADC_vect){
    static uint8_t drop_cnt = 0;
    if(drop_cnt < ADC_DROP_DATA){
        drop_cnt++;
        return;
    }
    else{
        drop_cnt = 0;
    
        switch(adc_now_ch){
            case ADC_L:
                adc_L_buffer[adc_L_buffer_index] = ADC;
                adc_L_buffer_index = (adc_L_buffer_index+1)%ADC_L_BUFFER_SIZE;
                // pre next ch 
                adc_now_ch = ADC_M;
                ADMUX=(1<<REFS0)|ADC_M;
                break;
            case ADC_M:
                adc_M_buffer[adc_M_buffer_index] = ADC;
                adc_M_buffer_index = (adc_M_buffer_index+1)%ADC_M_BUFFER_SIZE;
                // pre next ch
                adc_now_ch = ADC_R;
                ADMUX=(1<<REFS0)|ADC_R;
                break;
            case ADC_R:
                adc_R_buffer[adc_R_buffer_index] = ADC;
                adc_R_buffer_index = (adc_R_buffer_index+1)%ADC_R_BUFFER_SIZE;
                // pre next ch
                adc_now_ch = ADC_BAT;
                ADMUX=(1<<REFS0)|ADC_BAT;
                break;
            case ADC_BAT:
                adc_BAT_buffer[adc_BAT_buffer_index] = ADC;
                adc_BAT_buffer_index = (adc_BAT_buffer_index+1)%ADC_BAT_BUFFER_SIZE;
                // pre next ch
                adc_now_ch = ADC_L;
                ADMUX=(1<<REFS0)|ADC_L;
                break;
        }
    }
}


uint16_t adc_L_get_avg(){
    uint16_t avg = 0;
    for(int i=0;i<ADC_L_BUFFER_SIZE;i++){
        avg += adc_L_buffer[i];
    }
    avg /= ADC_L_BUFFER_SIZE;
    return avg;

}
uint16_t adc_M_get_avg(){
    uint16_t avg = 0;
    for(int i=0;i<ADC_M_BUFFER_SIZE;i++){
        avg += adc_M_buffer[i];
    }
    avg /= ADC_M_BUFFER_SIZE;
    return avg;
}
uint16_t adc_R_get_avg(){
    uint16_t avg = 0;
    for(int i=0;i<ADC_R_BUFFER_SIZE;i++){
        avg += adc_R_buffer[i];
    }
    avg /= ADC_R_BUFFER_SIZE;
    return avg;
}


//adc_BAT_to_volt(adc_BAT_get_avg())
uint16_t adc_BAT_get_avg(){
    uint16_t avg = 0;
    for(int i=0;i<ADC_BAT_BUFFER_SIZE;i++){
        avg += adc_BAT_buffer[i];
    }
    avg /= ADC_BAT_BUFFER_SIZE;
    return avg;
}

uint16_t adc_BAT_to_volt(uint16_t adc_val){
    return adc_val / 5 + 4; 
}
