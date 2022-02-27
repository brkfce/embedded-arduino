// https://www.robot-electronics.co.uk/i2c-tutorial
// https://www.exploreembedded.com/wiki/Basics_of_I2C_with_AVR
// https://embedds.com/programming-avr-i2c-interface
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <string.h>


// initialise two-wire interface (TWI)
// TWBR scales down CPU frequency for SCL 
// TWSR prescales the frequency with values 1,4,16,64
// TWCR enables TWI to interrupt, start, and stop
// TWDR is a data register used to hold the next byte to transmit or receive
// TWAR and TWARM are used when operating as a I2C slave
void TWIInit(void) {
	// set SCL rate to 400kHz
	TWSR = 0x00;	// prescaler bits
	TWBR = 0x0C;	// clock rate scaler
	// enable TWI
	TWCR = (1<<TWEN);
}

// generate start and stop signals
void TWIStart(void) {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	// need to wait for status (TWINT resetting zero)
	while ((TWCR & (1<<TWINT)) == 0);
}
void TWIStop(void) {
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
}

// write a data byte to the TWDR register, which is shifted to the SDA line
void TWIWrite(uint8_t u8data) {
	TWDR = u8data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for status
	while ((TWCR & (1<<TWINT)) == 0);
}

// read data from SDA line, either with or without acknowledgement
uint8_t TWIReadACK(void) {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}
uint8_t TWIReadNACK(void) {
	TWCR = (1<<TWINT) | (1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

// reading status
uint8_t TWIGetStatus(void) {
	uint8_t status;
	// mask status
	status = TWSR & 0xF8;
	return status;
}