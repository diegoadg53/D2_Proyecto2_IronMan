/*
 * com_uart.h
 *
 * Created: 3/2/2025 8:59:49 PM
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