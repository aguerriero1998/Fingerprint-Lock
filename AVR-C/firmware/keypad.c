#include "keypad.h"
#include <util/delay.h>
#include <avr/io.h>

uint8_t get_key(){
	uint8_t result = 0;
	
	DDRD |= 0xE0; //set columns as input
	DDRD &= 0xE3;

	DDRB |= 0x01; // and rows as output
	
	PORTD |= 0x1C;
	PORTD &= 0x1F;//turn on pull up resistors

	PORTB &= ~0x01; // and set outputs to ground

	_delay_loop_2(5); //let registers settle

	result = (PIND & 0x1C); // bit x + 1 will be 0 if column x is on 

	DDRD |= 0x1C; //set columns as output
	DDRD &= 0x1F;

	DDRB &= ~0x01; // and rows and input
    

	PORTD |= 0xE0; //turn on pull up resistors
	PORTD &= 0xE3;
	
	PORTB |= 0x01;
	
	_delay_loop_2(5); 

	result |= (PIND & 0xE0); //0x32132100 
	result = result >> 2; //0x00321321
	result |= ((PINB & 0x01) << 6) | (1 << 7); //0x04321321

	return ~result;

}
