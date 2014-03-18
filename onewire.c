/*
 * onewire.c
 *
 *  the simple one-wire library
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>

#include "defines.h"

unsigned char onewire_reset(void)
{
	W1_OUT &= ~(1 << W1_PIN); // One-wire LOW
	W1_DDR |= 1 << W1_PIN; // pin output

	_delay_us(480); // 480 us

	W1_DDR &= ~(1 << W1_PIN); // pin input
	W1_OUT |= (1 << W1_PIN); // pull-up

	_delay_us(65); // Wait 15 + 50 us

	unsigned char err = W1_IN & (1 << W1_PIN); // Sample bus

	_delay_us(415); // wait 480 - 65 until cycle is gone

	if ((W1_IN & (1 << W1_PIN)) == 0) // check bus for HIGH, otherwise we have a short circuit
		err = 1;

	return err;
}

unsigned char onewire_bit_read(void)
{
	W1_OUT &= ~(1 << W1_PIN); // One-wire LOW
	W1_DDR |= 1 << W1_PIN; // pin output

	_delay_us(1); // 1 us time

	W1_DDR &= ~(1 << W1_PIN); // pin input
	W1_OUT |= (1 << W1_PIN); // pull-up

	_delay_us(14); // 14 us

	unsigned char value = W1_IN & (1 << W1_PIN); // sample bus

	_delay_us(45); // 45 us

	return value;
}

void onewire_bit_write(unsigned char b)
{
	W1_OUT &= ~(1 << W1_PIN); // One-wire LOW
	W1_DDR |= 1 << W1_PIN; // pin output

	_delay_us(10); // 10 us

	if (b)
	{
		W1_DDR &= ~(1 << W1_PIN); // One-wire input
		W1_OUT |= (1 << W1_PIN); // pull-up
		_delay_us(5 + 45); // in total 60 us
	}
	else
	{
		_delay_us(5 + 45); // 60 us in total
		W1_DDR &= ~(1 << W1_PIN); // One-wire HIGH and use as input
		W1_OUT |= (1 << W1_PIN); // pull-up
	}
}

int onewire_read(unsigned char bits)
{
	unsigned char i = bits;
	int value = 1 << (bits - 1);
	int b = 0;

	do
	{
		b >>= 1;
		if (onewire_bit_read())
			b |= value;

	} while (--i);

	return b;
}

unsigned char onewire_byte_read(void)
{
	return (unsigned char) onewire_read(8);
}

void onewire_byte_write(unsigned char b)
{
	unsigned char i = 8;
	do
	{
		onewire_bit_write(b & 1);
		b >>= 1;

	} while (--i);
}

