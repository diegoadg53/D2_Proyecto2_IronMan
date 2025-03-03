/*
 * I2C.h
 *
 * Created: 3/1/2025 11:50:31 PM
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

//iniciar esclavo
void I2C_Slave_Init(uint8_t adress);

#endif /* I2C_H_ */