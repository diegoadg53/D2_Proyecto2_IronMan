/*
 * com_uart.h
 *
 * Created: 3/1/2025 10:34:22 PM
 *  Author: njfg0
 */ 

#ifndef COM_UART_H_
#define COM_UART_H_
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

void initUART9600(void);
void writetxtUART(char* texto);
void writeUART(char caracter);




#endif /* COM_UART_H_ */