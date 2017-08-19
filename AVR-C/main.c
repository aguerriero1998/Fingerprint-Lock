#include "main.h"
#include "uart.h"
#include "softuart.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "softuart.h"
#define __AVR_ATmega328P__ 1



 
int main(void)
{
	stdout = &uart_output;
	stdin = &uart_input;
 	softuart_init();
	//sei();

	uart_init();
	_delay_ms(200);
	printf("Working\n");

	char c;

	while(1){
		printf("Start packet\n");
		softuart_putchar(0xEF);
		softuart_putchar(0x01);

		softuart_putchar(0xFF);
		softuart_putchar(0xFF);
		softuart_putchar(0xFF);
		softuart_putchar(0xFF);

		softuart_putchar(0x01);
		softuart_putchar(0x00);
		softuart_putchar(0x03);
		softuart_putchar(0x01);
		softuart_putchar(0x00);
		softuart_putchar(0x05);

		char c = softuart_getchar();
		printf("Char %c\n",c);
	}


	return 0;
}  
