// sending data to a 1602 Serial LCD, using i2c
// https://www.mouser.com/pdfdocs/DFR0464Datasheet.pdf
// https://seeeddoc.github.io/Grove-LCD_RGB_Backlight/res/JHD1214Y_YG_1.0.pdf
// https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
// https://create.arduino.cc/projecthub/akshayjoseph666/interface-i2c-16x2-lcd-with-arduino-uno-just-4-wires-273b24
#include "i2c.h"
#define LCDADDR (uint8_t) 0b01001110 //0x27	// device i2c address

/* my old attempt
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
*/

// new attempt using https://www.youtube.com/watch?v=QyoLxOkJcJY

// prototypes
void lcd_init();	// LCD initialisation
void toggle();		// latching function of LCD
void lcd_cmd_hf(char v1);	// send half byte command to LCD
void lcd_cmd(char v2);	// send command to LCD
void lcd_dwr(char v3);	// send data to LCD
void lcd_msg(char *c);	// send string to LCD
void delay(int ms);	// delay
void lcd_lef_sh();	// left shift
void lcd_rig_sh();	// right shift

// latching function of LCD
void toggle() {
	TWDR |= 0x02;	// latching data in to LCD data register using high to low signal
	TWCR = (1<<TWINT) | (1<<TWEN);	// enable i2c and clear interrupt
	while (!(TWCR & (1<<TWINT)));
	delay(1);
	TWDR &= ~0x02;	
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

// send half byte command to LCD
void lcd_cmd_hf(char v1) {
	TWDR &= ~0x01;	// selecting register as command register
	TWCR = (1<<TWINT) | (1<<TWEN);	// enable i2c and clear interrupt
	while (!(TWCR & (1<<TWINT)));
	TWDR &= 0x0F;	// clearing the higher 4 bits
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	TWDR |= (v1 & 0x0F);	// masking higher 4 bits and sending to LCD
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	toggle();
}

// send command to LCD
void lcd_cmd(char v2) {
	TWDR &= ~0x01;	// selecting register as command register
	TWCR = (1<<TWINT) | (1<<TWEN);	// enable i2c and clear interrupt
	while (!(TWCR & (1<<TWINT)));
	TWDR &= 0x0F;	// clearing the higher 4 bits
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	TWDR |= (v2 & 0x0F);	// masking higher 4 bits and sending to LCD
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	toggle();
	TWDR &= 0x0F;	// clearing the higher 4 bits
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	TWDR |= ((v2 & 0x0F)<<4);	// masking lower 4 bits and sending to LCD
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	toggle();
}

// send data to LCD
void lcd_dwr(char v3) {
	TWDR |= 0x01;	// selecting register as command register
	TWCR = (1<<TWINT) | (1<<TWEN);	// enable i2c and clear interrupt
	while (!(TWCR & (1<<TWINT)));
	TWDR &= 0x0F;	// clearing the higher 4 bits
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	TWDR |= (v3 & 0x0F);	// masking higher 4 bits and sending to LCD
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	toggle();
	TWDR &= 0x0F;	// clearing the higher 4 bits
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	TWDR |= ((v3 & 0x0F)<<4);	// masking lower 4 bits and sending to LCD
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	toggle();
}

// LCD Initialisation
void lcd_init() {
	lcd_cmd_hf(0x30);	// sequence for init LCD
	lcd_cmd_hf(0x30);	// ''
	lcd_cmd_hf(0x30);	// ''
	lcd_cmd(0x28);		// selecting 16 x 2 LCD in 4 bit mode
	lcd_cmd(0x0C);		// display on, cursor off
	lcd_cmd(0x01);		// clear display
	lcd_cmd(0x06);		// cursor auto increment
	lcd_cmd(0x80);		// first line first location
}

// delay (this feels inefficient, but oh well)
void delay(int ms) {
	int i,j;
	for (i = 0; i < ms; i++) {
		for (j = 0; j <= 120; j++);
	}
}

// send string (this also feels...unsafe)
void lcd_msg(char *c) {
	while (*c != 0) {
		lcd_dwr(*c++);
	}
}

// right shift
void lcd_rig_sh() {
	lcd_cmd(0x1C);	// command for right shift
	delay(400);
}

// left shift
void lcd_lef_sh() {
	lcd_cmd(0x18);	// command for left shift
	delay(200);
}


int main(void) {
	char message[] = "LALALALA";
	TWIInit();
	TWIStart();
	TWIWrite(LCDADDR);
	lcd_init();
	lcd_msg(message);
}
