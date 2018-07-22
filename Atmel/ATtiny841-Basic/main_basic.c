/*
 * ATtiny841_Basic.c
 *
 * Created: 2/15/2015 5:50:42 AM
 *  Author: Alan Uomoto

FUSES
Reading the fuses:
	avrdude -c usbtiny -p attiny841 -U lfuse:r:-:h

For an unmodified ATtiny841, this gives:
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

CLOCK SPEED 14.745600 MHz

To use a 14.745600 MHz crystal, or any crystal frequency over 8 MHz,
change the CKSEL[3:0] bits (p 26 & p 220) to 111X.

So lfuse becomes 0xce:

avrdude -c usbtiny -p attiny841 -U lfuse:w:0xCE:m

USART
There are two:
	USART0: RXD0 is PA2 on pin 11, TXD0 is PA1 on pin 12
	USART1: RXD1 is PA4 on pin 9, TXD1 is PA5 on pin 8

 */ 


/*
#define F_CPU 14745600
*/
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

#define BAUDRATE	9600
#define MYUBRR		((F_CPU / 16 / BAUDRATE) - 1)
#define TX0READY	(UCSR0A & (1 << UDRE0))
#define TX1READY	(UCSR1A & (1 << UDRE1))

// Function prototypes
void flashLED(uint8_t);
void initialize(void);
void serial0SendByte(uint8_t);
void serial1SendByte(uint8_t);

int main(void)
{

	initialize();

	for (;;) {
		serial0SendByte('a');
		flashLED(1);
		_delay_ms(1000);
	}

}

void initialize()
{

	// Setup serial port 0
	UBRR0H = (uint8_t) (MYUBRR >> 8);		// Set the baud rate
	UBRR0L = (uint8_t) MYUBRR;				// Set the baud rate (second half)
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);	// Enable USART receive and transmit
	UCSR0C = (3 << UCSZ00);					// 8 bits, no parity, one stop bit

	// Setup serial port 1
	UBRR1H = (uint8_t) (MYUBRR >> 8);		// Set the baud rate
	UBRR1L = (uint8_t) MYUBRR;				// Set the baud rate (second half)
	UCSR1B = (1 << RXEN0) | (1 << TXEN0);	// Enable USART receive and transmit
	UCSR1C = (3 << UCSZ00);					// 8 bits, no parity, one stop bit

	// Set up the LED on the ATtiny841 Basic board
	DDRB |= (1 << PORTB2);					// LED is on PORTB2

	flashLED(5);

}

void flashLED(uint8_t ntimes)
{

	uint8_t i;

	for (i = 0; i < ntimes; i++) {
		PORTB |= (1 << PORTB2);
		_delay_ms(125);
		PORTB &= ~(1 << PORTB2);
		_delay_ms(125);
	}

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