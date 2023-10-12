/*
 * Firmware for ATTINY13A MCU to control gas valve
 * in Paslode IM100C vendor lock-in override device.
 *
 * Copyright (c) 2023 Ruslan Zalata.
 *
 */

#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define GAS_ON_TIME_MS	14
#define GAS_OFF_TIME_MS	500

int main(void)
{
	DDRB |= (1<<DDB2); // Output: Heartbeat LED
	PORTB &= ~(1<<PB2); // Turn OFF LED

	DDRB |= (1<<DDB3); // Output: Gas valve
	PORTB &= ~(1<<PB3); // Close Gass valve (active high)

	DDRB &= ~(1<<DDB0); // Input: Shutter trigger 
	PORTB |= (1<<PB0); // Enable pull-up for Shutter trigger

	DDRB &= ~(1<<DDB1); // Input: User button 
	PORTB |= (1<<PB1); // Enable pull-up for User button

	PCMSK = _BV (PCINT0); // Enable external interrupts PCINT0
	MCUCR = _BV (ISC01); // The falling edge of PCINT0 generates IRQ 
	GIMSK = _BV (PCIE); // Pin Change Interrupt Enable

	sei();

	while (1) {
		// Toggle Heartbeat LED once a second 
		PORTB ^= (1<<PB2);
		_delay_ms(1000);
	}

	return 0;
}

ISR (PCINT0_vect)
{
	cli();

	if ((PINB & (1<<PB0)) == 0) {
		PORTB |= (1<<PB2); // Heartbeat goes LED ON when gas valve open
		PORTB |= (1<<PB3); // Open gas valve	
		_delay_ms(GAS_ON_TIME_MS);
		PORTB &= ~(1<<PB3); // Close gas valve	
		_delay_ms(GAS_OFF_TIME_MS);
		PORTB &= ~(1<<PB2); // Heartbeat is LED goes OFF 

		GIFR |= 0b01100000;
	}

	sei();
}

