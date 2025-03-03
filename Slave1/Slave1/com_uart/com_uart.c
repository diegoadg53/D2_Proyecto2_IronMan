/*
 * com_uart.c
 *
 * Created: 3/1/2025 10:33:55 PM
 *  Author: njfg0
 */ 
#include "com_uart.h"
void initUART9600(void){
	//configurar pines tx y rx
	DDRD &=~(1<<DDD0);//entrada
	DDRD|=(1<<DDD1);//salida Tx
	
	//configurar registro A modo fast u2x0 en 1
	UCSR0A=0;
	UCSR0A|=(1<<U2X0);
	
	//configurar registro B, habilotar isr rx, bits para habilitar RX y TX
	UCSR0B=0;
	UCSR0B|=(1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	
	//Configurar registro C<Frame asincrono 8 bits de datos sin pariedad, 1 bit de stop
	UCSR0C=0;
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);
	
	//baudrate = 207 con % de error igual a 0.16% con 9600
	UBRR0=207;
	
}

void writetxtUART(char* texto){
	uint8_t i;
	for (i=0;texto[i]!='\0';i++){
		while(!(UCSR0A&(1<<UDRE0)));//esperar hasta que el udre0 esté en 1
		UDR0=texto[i];//cuando i nulo se acaba
	}
}

void writeUART(char caracter){
	while(!(UCSR0A&(1<<UDRE0)));//esperar hasta que el udre0 esté en 1
	UDR0=caracter;//valor que queremos enviar
	
}