// sending data to a 1602 Serial LCD, using i2c
// https://www.mouser.com/pdfdocs/DFR0464Datasheet.pdf
#include "i2c.h"

#define DEVADDR 0x7C	// device i2c address
enum status { SUCCESS = 0, ERROR = 1};

uint8_t LCDWriteByte(uint16_t u16addr, uint8_t u8data) {
	TWIStart();
	if (TWIGetStatus() != 0x08) return ERROR;
	// select device and send A2 A1 A0 address bits
	TWIWrite((DEVADDR) | ((uint8_t)((u16addr & 0x0700)>>7)));
	if (TWIGetStatus() != 0x18) return ERROR;
	// send the rest of the address
	TWIWrite((uint8_t) (u16addr));
	if (TWIGetStatus() != 0x28) return ERROR;
	// write byte to LCD
	TWIWrite(u8data);
	if (TWIGetStatus() != 0x28) return ERROR;
	TWIStop();
	return SUCCESS;
}

int main(void) {

}