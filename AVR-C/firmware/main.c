#include "main.h"
#include "uart.h"

 
int main(void)
{
	uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	char password[4];
	uint8_t cnt = 0;
	uint8_t key = 0;
	uint8_t last_key = 0;
	uint8_t released;

	DDRB |= (1<<PB4);
	DDRB |= (1<<PB5);
	while(1){
		key = get_key();

		released = !key && last_key;
		if(released){
			password[cnt] = last_key;
			cnt++;
			UDR0 = last_key + 48;
			if(last_key == 0x09){
				PORTB |= 1<<PB5;
			}
		PORTB |= (1 << PD4);
		}else{
			PORTB &= ~(1 << PD4);
		}
		if(cnt == 4){
			if((password[0] == 0x09) && (password[1] == 0x09) && (password[2] == 0x09) && (password[3] == 0x09)){
				PORTB |= (1 << PB5);
				_delay_ms(1000);
			}else{
				PORTB |= (1 << PB4);
			}
			_delay_ms(500);
			PORTB &= ~(1 << PB5);
			cnt = 0;
		}
		UDR0 = key + 48;
		_delay_ms(16);
		last_key = key;
	}
	return 0;
 }  
