// https://thewanderingengineer.com/2014/08/11/arduino-pin-change-interrupts/

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>



int main() {

	// need to turn on "Pin Change Interrupts"
	// PCICR |= 0b00000001;	// turn on port b
	// PCICR |= 0b00000010;	// turn on port c
	 PCICR |= 0b00000100;	// turn on port d
	// PCICR |= 0b00000111;	// turn on all ports

	// need to choose which pins to interrupt
	// PCMSK0 |= 0b00000001;	// turn on pin PB0, which is PCINT0, physical pin 14
	// PCMSK1 |= 0b00010000;	// turn on pin PC4, which is PCINT12, physical pin 27
	PCMSK2 |= 0b10000000;	// turn on pin PD7, PCINT23

	// put B5 in output mode
	DDRB |= 0b00100000;

	// globally enable interrupts
	sei();

	while(true) {}

}

// need to write the ISR
// ISR(PCINT0_vect){}	// port B, PCINT0 - PCINT7
//ISR(PCINT1_vect){}	// port C, PCINT8 - PCINT14
ISR(PCINT2_vect){		// port D, PCINT16 - PCINT23
	PORTB ^= 0b00100000;	// toggles PORTB register, blinking the LED
}