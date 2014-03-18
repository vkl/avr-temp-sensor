/*
 * main.c
 *
 *  Created on: Mar 3, 2014
 *      Author: kladov
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "onewire.h"
#include "ds1821.h"

volatile char buffer[10] = { 0 };

#define LEDPIN PB0

void ioinit()
{
	DDRB |= (1 << LEDPIN);
	TCNT1 = 0;
	//TCCR1B |= (1 << CS12) | (1 << CS10); // prescaler 1024
	//TCCR1B |= (1 << CS12); // prescaler 256
	TCCR1B |= (1 << CS11) | (1 << CS10); // prescaler 64
	TIMSK |= (1 << TOIE1); // enable timer/counter1 overflow interrupt
	sei();
}

void read_temp()
{
	uint8_t t = 0;
	uint8_t c = 0;
	uint8_t r = 0;
	uint8_t status = 0;
	int i;
	long temperature = 0;
	onewire_reset();
	onewire_byte_write(STARTCONV); // start

	for (i = 0; i < 50; i++)
	{
		_delay_ms(500);
		onewire_reset();
		status = onewire_byte_read();
		if (status & (1 << DONE))
		{
			break;
		}
	}

	onewire_reset();
	onewire_byte_write(READTEMP); // read temp
	t = onewire_byte_read(); // low byte

	onewire_reset();
	onewire_byte_write(READCNTR); // read remaining count
	r = onewire_read(9);

	onewire_reset();
	onewire_byte_write(LOADCNTR); // load counter

	onewire_reset();
	onewire_byte_write(READCNTR); // read slope counter
	c = onewire_read(9);

	//sprintf(buffer, "RAW  :  %03d %03d %03d", t, r, c );
	if (t > 0x80)
	{
		t = t - 256;
	}

	temperature = 10 * t - 5;
	temperature += (10 * (c - r)) / c;

	t = temperature / 10;
	r = temperature % 10;

	//sprintf( buffer, "TEMP :  %3d.%d C", t, r );
}

// timer for delay
ISR (TIMER1_OVF_vect)
{
	if (PINB & (1 << LEDPIN))
	{
		PORTB &= ~(1 << LEDPIN);
	}
	else
	{
		PORTB |= (1 << LEDPIN);
		read_temp();
	}
}

int main(void)
{
	ioinit();

	for (;;)
	{
		sleep_mode();
	}

	return 0;
}

