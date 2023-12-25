#include <avr/io.h>

#include "LCD.h"

unsigned char 	Temp_CMD ;
unsigned char 	Str_Temp ;
unsigned char	Out_Mask ;

void lcd_init(){
	uint8_t tmpData;
	DIR_LCD_DATA = DIR_LCD_DATA | 0xF7;	// LCD DB[4:7} & RS & R/W & E are output function
	tmpData = PORTA;
	tmpData &= ~((1<<LCD_E) | (1<<LCD_RS) | (1<<LCD_RW));
	PORTA = tmpData;
	LCD_DATA = 0x00;					// LCD DB[4:7] & RS & R/W --> Low

	// _delay_ms(15);						// Set E pin as output
	lcd_l_delay();

	LCD_DATA = 0x30;	//0b00110000   // Setup for 4-bit Data Bus Mode
	lcd_CMD_W_Timing();
	// _delay_ms(6);
	lcd_l_delay();

	LCD_DATA = 0x30;	//0b00110000 
	lcd_CMD_W_Timing();
	// _delay_us(100);
	lcd_s_delay();

	LCD_DATA = 0x30;	//0b00110000
	lcd_CMD_W_Timing();
	// _delay_ms(1);
	lcd_s_delay();

	LCD_DATA = 0x20;	//0b00100000 ;
	lcd_CMD_W_Timing();
	// _delay_ms(1);
	lcd_s_delay();

	WriteCmdLCD(DISP_2Line_4Bit);
	// _delay_ms(1);
	lcd_s_delay();

	WriteCmdLCD(DISP_ON);
	// _delay_ms(1);
	lcd_s_delay();

	WriteCmdLCD(CLR_DISP);
	// _delay_ms(1);
	lcd_s_delay();

	WriteCmdLCD(ENTRY_INC);
	// _delay_ms(1);
	lcd_s_delay();	
}

// Write Command to LCD module
void WriteCmdLCD( uint8_t LCD_CMD){
	Temp_CMD = (LCD_CMD & 0xF0);
	LCD_DATA = (LCD_DATA & 0x0F)|Temp_CMD;
	lcd_CMD_W_Timing();
	Temp_CMD = (LCD_CMD & 0x0F)<<4;			// Send low nibble to LCD bus
	LCD_DATA = (LCD_DATA & 0x0F)|Temp_CMD;
	lcd_CMD_W_Timing ();

	lcd_s_delay();
	// _delay_ms(1);
}

// Write Data to LCD module
void WriteDataLCD( uint8_t LCD_CMD){
	Temp_CMD = (LCD_CMD & 0xF0);
	LCD_DATA = (LCD_DATA & 0x0F)|Temp_CMD;
	lcd_DAT_W_Timing();

	Temp_CMD = (LCD_CMD & 0x0F)<<4;			// Send low nibble to LCD bus
	LCD_DATA = (LCD_DATA & 0x0F)|Temp_CMD;
	lcd_DAT_W_Timing();

	lcd_s_delay();
	// _delay_ms(1);
}


void lcd_CMD_W_Timing( void ){
	uint8_t tmpTiming;
	tmpTiming = PORTA;
	tmpTiming &= ~((1<<LCD_RS)|(1<<LCD_RW));	// Set for Command Input
	tmpTiming |= (1<<LCD_E);
	PORTA = tmpTiming;
	asm volatile("nop\n\t""nop\n\t"::);			//Nop();
	tmpTiming &= ~(1<<LCD_E);
	PORTA = tmpTiming;
}

void lcd_DAT_W_Timing( void ){
	uint8_t tmpTiming;
	tmpTiming = PORTA;
	tmpTiming |= (1<<LCD_RS);					// Set for Data Input
	tmpTiming &= ~(1<<LCD_RW);
	tmpTiming |= (1<<LCD_E);
	PORTA = tmpTiming;
	asm volatile("nop\n\t""nop\n\t"::);			//Nop();
	tmpTiming &= ~(1<<LCD_E);
	PORTA = tmpTiming;
}




//*************************************************************************************************************
//******************************************* public function ************************************************
//*************************************************************************************************************


// Set Cursor position on LCD module
// CurY = Line (0 or 1)
// CurX = Position ( 0 to 15)
void lcd_set_cursor(uint8_t CurY, uint8_t CurX){
	WriteCmdLCD( 0x80 + CurY * 0x40 + CurX);
	//_delay_us(150);
	asm volatile("nop\n\t");
	asm volatile("nop\n\t");
	asm volatile("nop\n\t");
	asm volatile("nop\n\t");
}

void lcd_print(char *Str){
	while (1){
		Str_Temp = *Str;
		if (Str_Temp != 0x00 ){
			WriteDataLCD(Str_Temp);
			Str ++;
		}
		else{
			return;
		}
	}
}

void lcd_print_char(uint8_t LCD_Char){
	WriteDataLCD(LCD_Char);
}

void lcd_print_hex(uint16_t HEX_Val){
	uint16_t Temp_HEX;

	Temp_HEX = (HEX_Val >> 12) & 0x0f;
	if ( Temp_HEX > 9 )Temp_HEX += 0x37;
    else Temp_HEX += 0x30;
	WriteDataLCD(Temp_HEX);

	Temp_HEX = (HEX_Val >> 8) & 0x0f;
	if ( Temp_HEX > 9 )Temp_HEX += 0x37;
    else Temp_HEX += 0x30;
	WriteDataLCD(Temp_HEX);

	Temp_HEX = (HEX_Val >> 4) & 0x0f;
	if ( Temp_HEX > 9 )Temp_HEX += 0x37;
    else Temp_HEX += 0x30;
	WriteDataLCD(Temp_HEX);

	Temp_HEX = HEX_Val  & 0x0f;
	if ( Temp_HEX > 9 )Temp_HEX += 0x37;
    else Temp_HEX += 0x30;
	WriteDataLCD(Temp_HEX);
}

void lcd_l_delay(void){
	uint8_t i, j;
	for (i=0; i<255; i++)	
	{
		for (j=0; j<255; j++)	
		{
			asm volatile("nop\n\t");
		}
	}		
}

void lcd_s_delay(void){
	uint8_t i, j;
	for (i=0; i<10; i++)	
	{
		for (j=0; j<255; j++)	
		{
			asm volatile("nop\n\t");
		}
	}		
}

void lcd_clear(void){
   WriteCmdLCD(0x01);     //  this clear display all 
   _delay_ms(1);
}

void lcd_print_int(int val,uint8_t field_length){    //val display num  field for cloumd
	char str[5] = {0,0,0,0,0};
	uint8_t i = 4, j = 0;
	while(val){
		str[i] = val % 10;
		val = val / 10;
		i--;
	}
	if(field_length > 51){
		while(str[j]==0) j++;
	}
	else{
		j = 5 - field_length;
	}

	if(val<0){
		WriteDataLCD('-');
	}
	for(i=j;i<5;i++){
    	WriteDataLCD('0'+str[i]);
	}
}

