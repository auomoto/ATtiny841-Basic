/*
 * ATtiny841_Basic.c
 *
 * Created: 2/15/2015 5:50:42 AM
 *  Author: Alan Uomoto

FUSES
Reading the fuses:
	avrdude -c usbtiny -p attiny841 -U lfuse:r:-:h

gives:
	lfuse = 0x42
	hfuse = 0xdf
	efuse = 0xff

We noticed that the lfuse value listed as default in the datasheet
(p 219) is 0x62. Bit 5, which is not used, is programmed (value = 0)
on my device but the datasheet says it's not (value = 1).

CLOCK SPEED 8 MHz
The factory default divides the internal clock by 8, resulting in a
1 MHz clock speed. I changed this so that the chip runs at 8 MHz (no
clock division) by changing lfuse to 0xC2.
	avrdude -c usbtiny -p attiny841 -U lfuse:w:0xC2:m
so:
	lfuse = 0xc2
	hfuse = 0xdf
	efuse = 0xff

CLOCK SPEED 14.7 MHz
Using a 14.7 MHz crystal, change the CKSEL[3:0] bits (p 26 & p 220) to 111X.
So lfuse become 

USART
There are two:
	USART0: RXD0 is PA2 on pin 11, TXD0 is PA1 on pin 12
	USART1: RXD1 is PA4 on pin 9, TXD1 is PA5 on pin 8



 */ 

#include <avr/io.h>
#include <util/delay.h>

#define LEDDDR	DDRB
#define LEDPIN	PB2

#define BAUDRATE	9600
#define MYUBRR		((F_CPU / 16 / BAUDRATE) - 1)
#define TX0READY	(UCSR0A & (1 << UDRE0))
#define TX1READY	(UCSR1A & (1 << UDRE1))

void initialize(void);
void serial0SendByte(uint8_t);
void serial1SendByte(uint8_t);

int main(void)
{

	initialize();

	for (;;) {
		PINB = _BV(LEDPIN);
		serial1SendByte('b');
		_delay_ms(500);
		serial1SendByte('0');
		_delay_ms(500);
	}

}

void initialize()
{

	LEDDDR |= _BV(LEDPIN);				// LEDPIN is output

	// Setup serial0
	UBRR0H = (uint8_t) (MYUBRR >> 8);	// Baud rate
	UBRR0L = (uint8_t) MYUBRR;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ00);				// 8 bits, no parity, one stop bit

	// Setup serial1
	UBRR1H = (uint8_t) (MYUBRR >> 8);	// Baud rate
	UBRR1L = (uint8_t) MYUBRR;
	UCSR1B = (1 << RXEN0) | (1 << TXEN0);
	UCSR1C = (3 << UCSZ00);				// 8 bits, no parity, one stop bit

}

void serial0SendByte(uint8_t c)
{

	while (!TX0READY) {
		asm("nop");
	}
	UDR0 = c;

}

void serial1SendByte(uint8_t c)
{

	while (!TX1READY) {
		asm("nop");
	}
	UDR1 = c;

}