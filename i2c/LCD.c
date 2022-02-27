// sending data to a 1602 Serial LCD, using i2c
// https://www.mouser.com/pdfdocs/DFR0464Datasheet.pdf
// https://seeeddoc.github.io/Grove-LCD_RGB_Backlight/res/JHD1214Y_YG_1.0.pdf
// https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
// https://create.arduino.cc/projecthub/akshayjoseph666/interface-i2c-16x2-lcd-with-arduino-uno-just-4-wires-273b24
#include "i2c.h"

#define LCDADDR (uint8_t) 0b01001110 //0x27	// device i2c address
enum status {SUCCESS = 0, BUS_ERROR = 1, FUNC_ERROR = 2};
enum display_commands {
	READWRITE = 0b00000001, 
	WRITING = 0b01000000, 
	NOTLAST = 0b10000000, 
	CLEAR = 0b00000001, 
	DISPLAY = 0b00001100,
	FUNCTIONSET = 0b00111000,
	ENTRYMODE = 0b00000110};
enum delays {
	POWERON = 30,
	FSET = 1,
	DSP = 1,
	CLR = 2};


uint8_t LCDTransferData(uint8_t u8addr, char * u8data_pointer, int byte_count) {
	if (byte_count <= 0 || byte_count > 32) return FUNC_ERROR;
	
	// poweron delay
	_delay_ms(POWERON);

	TWIStart();
	if (TWIGetStatus() != 0x08) return BUS_ERROR;
	
	// select device 
	TWIWrite(u8addr &= ~READWRITE);	// change to |= to read 
	if (TWIGetStatus() != 0x18) return BUS_ERROR;
	
	// function set
	TWIWrite(NOTLAST);
	if (TWIGetStatus() != 0x28) return BUS_ERROR;
	TWIWrite(FUNCTIONSET);
	if (TWIGetStatus() != 0x28) return BUS_ERROR;
	_delay_ms(FSET);

	// turn on display
	TWIWrite(NOTLAST);
	if (TWIGetStatus() != 0x28) return BUS_ERROR;
	TWIWrite(DISPLAY);
	if (TWIGetStatus() != 0x28) return BUS_ERROR;
	_delay_ms(DSP);

	// clear screen
	TWIWrite(NOTLAST);
	if (TWIGetStatus() != 0x28) return BUS_ERROR;
	TWIWrite(CLEAR);
	if (TWIGetStatus() != 0x28) return BUS_ERROR;
	_delay_ms(CLR);

	// set entry mode
	TWIWrite(NOTLAST);
	if (TWIGetStatus() != 0x28) return BUS_ERROR;
	TWIWrite(ENTRYMODE);
	if (TWIGetStatus() != 0x28) return BUS_ERROR;

	
	//write screen data
	for (int byte_counter = 0; byte_counter < byte_count; byte_counter++) {
		if (byte_counter != byte_count -1) {
			TWIWrite(NOTLAST | WRITING);
			if (TWIGetStatus() != 0x28) return BUS_ERROR;
		}
		else {
			TWIWrite(WRITING);
			if (TWIGetStatus() != 0x28) return BUS_ERROR;
		}
		TWIWrite(u8data_pointer[byte_counter]);
		if (TWIGetStatus() != 0x28) return BUS_ERROR;
	}

	TWIStop();
	return SUCCESS;
}


int main(void) {
	char message[] = "LALALALA";
	LCDTransferData(LCDADDR, &message[0], strlen(message));
}