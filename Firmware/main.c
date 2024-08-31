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
#include <avr/wdt.h>

#define GAS_OFF_TIME_MS	500U
#define	VREF		3300UL	// Vref = Vcc = 3.3V

unsigned long sensor_mv = 0;
unsigned int pulse_ms = 14;

// ADC at -30C = 195mV
// ADC at -20C = 593mV 
// ADC at -10C = 920mV
// ADC at   0C = 2153mV
// ADC at +20C = 2244mV
// ADC at +30C = 2398mv

struct _temp_to_pulse {
	unsigned long adc_mv;
	unsigned int pulse_ms;
} temp_to_pulse[] = {
	{0, 42}, // at -60C
	{195, 42}, // at -30C
	{593, 36}, // at -20C
	{920, 20}, // at -10C
	{2153, 18}, // at 0C
	{2244, 14}, // at +20C
	{2398, 10}, // at +30C
	{3300, 10}, // sentinel
};

unsigned int calc_pulse(unsigned long adc_mv) {
	int i;
	unsigned int cur_pulse_ms = 0;

	if(adc_mv < 5)
		return 0; // Something is wrong with NTC sensor

	for(i = 0; i < sizeof(temp_to_pulse) / sizeof(temp_to_pulse[0]); i++)
		if(adc_mv >= temp_to_pulse[i].adc_mv)
			cur_pulse_ms = temp_to_pulse[i].pulse_ms;

	return cur_pulse_ms;
}

int main(void)
{
	cli();

	DDRB |= (1<<DDB2); // Output: Heartbeat LED
	PORTB &= ~(1<<PB2); // Turn OFF LED

	DDRB |= (1<<DDB3); // Output: Gas valve
	PORTB &= ~(1<<PB3); // Close Gass valve (active high)

	DDRB &= ~(1<<DDB0); // Input: Shutter trigger 
	PORTB |= (1<<PB0); // Enable pull-up for Shutter trigger

	DDRB &= ~(1<<DDB1); // Input: User button 
	PORTB |= (1<<PB1); // Enable pull-up for User button

	// PB4 (ADC2) pin is input
	DDRB &= ~(1<<DDB4); // Input: ADC2 
	PORTB &= ~(1<<PB4); // No pull-up for ADC2 
	// Set the ADC mux to ADC2, enable left adjust, Vref = Vcc
	ADMUX = (1 << MUX1) | (1 << ADLAR);
	// Set the prescaler to clock/128, enable ADC, no IRQ
	ADCSRA = (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

	PCMSK = _BV (PCINT0); // Enable external interrupts PCINT0
	MCUCR = _BV (ISC01); // The falling edge of PCINT0 generates IRQ 
	GIMSK = _BV (PCIE); // Pin Change Interrupt Enable

	// Enable Watchdog
        WDTCR |= (1<<WDCE) | (1<<WDE); // Allow WDT reconfig, then enable WDT
	WDTCR |= (1<<WDCE) | (1<<WDP3); // Allow WDT reconfig, then set timeout to 4s
	wdt_reset();

	sei();

	// Long blink on statup
	PORTB |= (1<<PB2);
	_delay_ms(2000);
	PORTB &= ~(1<<PB2);
	wdt_reset();

	while (1) {
		// Start new the conversion
		ADCSRA |= (1 << ADSC);

		// Toggle Heartbeat LED once a second 
		PORTB ^= (1<<PB2);

		// Ping watchdog 
		wdt_reset();

		_delay_ms(1000);

		// Wait ADC to complete conversion
		while (ADCSRA & (1 << ADSC));

		// Read, cals and average temperature
		sensor_mv = VREF * (unsigned long)ADCH / 256;

		// Critical section
		cli();
		pulse_ms = calc_pulse(sensor_mv);
		sei();
	}

	return 0;
}

ISR (PCINT0_vect)
{
	cli();

	if ((PINB & (1<<PB0)) == 0) {
		int i;
	
		PORTB |= (1<<PB2); // Heartbeat goes LED ON when gas valve open
		PORTB |= (1<<PB3); // Open gas valve	

		// Keep pulse as long as pulse_ms miliseconds
		for(i = 0; i < pulse_ms; i++)
			_delay_ms(1);

		PORTB &= ~(1<<PB3); // Close gas valve	
		_delay_ms(GAS_OFF_TIME_MS);
		PORTB &= ~(1<<PB2); // Heartbeat is LED goes OFF 

		GIFR |= 0b01100000;
	}

	sei();
}


ISR (WDT_vect) {
}

