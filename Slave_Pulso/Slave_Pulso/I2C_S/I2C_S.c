/*
 * I2C_S.c
 *
 * Created: 3/2/2025 9:00:10 PM
 *  Author: diego
 */ 

#include "I2C_S.h"

void I2C_Slave_Init(uint8_t address){
	DDRC &= ~((1<<DDC4)|(1<<DDC5)); //Pines de I2C como entradas
	
	TWAR = address << 1; // Se asigna la dirección que tendrá
	
	TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWIE); //Se habilita la interfaz, ACK automático, se Habilita la ISR
}