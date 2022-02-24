#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#define PRESCALER 10	// set to divide by 1024, which is 10 bits
#define CLOCK_RATE 15998976	// clock rate of microcontroller
#define COUNTER_VALUE (CLOCK_RATE >> PRESCALER)	// roughly the number of ticks in a second

// TIMER1 compare "interrupt service routine" (ISR) fires when counter value matches OCR1A
ISR(TIMER1_COMPA_vect) {
	PORTB ^= _BV(PORTB);	// toggles PORTB register
}

int main () {

	// put B5 in output mode
	DDRB |= _BV(PORTB5);

	// configure timer
	OCR1A = COUNTER_VALUE;	// output compare register for timer 1

	TCCR1A = 0x00;	// timer counter configuration register for timer 1
	TCCR1B |= (1 << WGM12);	// CTC mode - compare to OCR1A, and clear counter & interrupt when it matches
	TCCR1B |= (1 << CS10) | (1 << CS12);	// set prescaler to 1024

	// create an interrupt
	TIMSK1 |= (1 << OCIE1A);

	// globally enable interrupts
	sei();

	while (true) {
		// do other useful stuff while waiting for the timer
	}

}