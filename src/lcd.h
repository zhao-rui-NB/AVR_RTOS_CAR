#ifndef __LCD_H__
#define __LCD_H__

// #include <inttypes.h>

#include <avr/io.h>
#include <util/delay.h>

// Definitions for I/O ports that provide LCD data & control
// PORTA[4:7]-->DB[4:7]: Higher order 4 lines data bus with bidirectional
//					  : DB7 can be used as a BUSY flag
// PORTA,0 --> [RS]: LCD Register Select control
// PORTA,1 --> [RW]: LCD Read/Write control
// PORTA,2 --> [E] : LCD operation start signal control 
//		      	   : "0" for Instrunction register (Write), Busy Flag (Read)
//				   : "1" for data register (Read/Write)
//
#define LCD_RS			PA0			// The definition of control pins
#define LCD_RW			PA1
#define LCD_E			PA2
#define DIR_LCD_RS		DDA0
#define DIR_LCD_RW		DDA1
#define DIR_LCD_E		DDA2

#define LCD_DATA		PORTA		// PORTA[4:7] as LCD DB[4:7]
#define DIR_LCD_DATA	DDRA

//  LCD Module commands
#define DISP_2Line_8Bit	0x38		//0b00111000
#define DISP_2Line_4Bit	0x28		//0b00101000
#define DISP_ON			0x0C		// Display on
#define DISP_ON_C		0x0E		// Display on, Cursor on
#define DISP_ON_B		0x0F		// Display on, Cursor on, Blink cursor
#define DISP_OFF		0x08		// Display off
#define CLR_DISP		0x01		// Clear the Display
#define ENTRY_INC		0x06		//
#define ENTRY_INC_S		0x07		//
#define ENTRY_DEC		0x04		//
#define ENTRY_DEC_S		0x05		//
#define DD_RAM_ADDR		0x80		// Least Significant 7-bit are for address
#define DD_RAM_UL		0x80		// Upper Left coner of the Display	



void lcd_init(void);

void WriteCmdLCD( uint8_t LCD_CMD);

void WriteDataLCD( uint8_t LCD_CMD);

void lcd_print_char(uint8_t LCD_Char);

void lcd_CMD_W_Timing( void );

void lcd_DAT_W_Timing( void );

void lcd_set_cursor(uint8_t CurY, uint8_t CurX);

//void putrsLCD( const rom char *Str );

void lcd_print( char *Str);

void lcd_print_hex(uint16_t HEX_Val);

void lcd_s_delay(void);

void lcd_l_delay(void);

void lcd_clear(void);

void lcd_print_int(int val,uint8_t field_length);

#endif // __LCD_H__
