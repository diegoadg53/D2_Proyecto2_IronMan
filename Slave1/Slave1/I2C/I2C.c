/*
 * I2C.c
 *
 * Created: 3/1/2025 11:50:22 PM
 *  Author: njfg0
 */ 

#include "I2C.h"
void I2C_Slave_Init(uint8_t adress){
	DDRC&=~((1<<DDC4)|(1<<DDC5));//ENTRADAS
	TWAR=adress<<1;//DIRECCIÓN
	TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWIE);//HABILITA INTERFAZ
}