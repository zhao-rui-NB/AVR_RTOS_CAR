#include <avr/io.h>
#include <util/delay.h>


/*
led pin 
0: PG0
1: PG1
2: PG2
3: PG3
4: PG4
*/


int main(void){
    DDRG = 0xFF;
    PORTG = 0x00;

    while (1)
    {
        PORTG++;
        _delay_ms(100);
    }
}
