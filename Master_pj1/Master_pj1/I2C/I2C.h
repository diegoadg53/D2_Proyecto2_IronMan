/*
 * I2C.h
 *
 * Created: 2/27/2025 4:01:13 PM
 *  Author: njfg0
 */ 


#ifndef I2C_H_
#define I2C_H_

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

//INICIAR MAESTRO I2C
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler);
//inicio comunicacion
void I2C_Master_Start(void);
//parar comunicacion
void I2C_Master_Stop(void);
//escribir
uint8_t I2C_Master_Write(uint8_t dato);
//leer
uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack);
//iniciar esclavo
void I2C_Slave_Init(uint8_t adress);





#endif /* I2C_H_ */