/*
 * I2C.c
 *
 * Created: 2/27/2025 4:02:23 PM
 *  Author: njfg0
 */ 
#include "I2C.h"
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler){
	DDRC&=~((1<<DDC4)|(1<<DDC5));
	
	switch(Prescaler){
		case 1:
		TWSR&=~((1<<TWPS1)|(1<<TWPS0));
		break;
		case 4:
		TWSR&=~(1<<TWPS1);
		TWSR|=(1<<TWPS0);
		break;
		case 16:
		TWSR&=~(1<<TWPS0);
		TWSR|=(1<<TWPS1);
		break;
		case 64:
		TWSR|=((1<<TWPS1)|(1<<TWPS0));
		break;
		default:
		TWSR&=~((1<<TWPS1)|(1<<TWPS0));
		Prescaler=1;
		break;
	}
	TWBR=((F_CPU/SCL_Clock)-16)/(2*Prescaler);//velocidad que queremos
	TWCR|=(1<<TWEN);//Habilitamos comunicacion
}

void I2C_Master_Start(void){
	//uint8_t estado;
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while(!(TWCR &(1<<TWINT))); //espera a que termine la bandera
	
}

void I2C_Master_Stop(void){
	TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

uint8_t I2C_Master_Write(uint8_t dato){
	
	uint8_t estado;
	
	TWDR=dato; //carga texto
	TWCR=(1<<TWEN)|(1<<TWINT);//inicia el envio
	while(!(TWCR &(1<<TWINT)));
	estado=TWSR&0xF8;//verifica estado
	if(estado==0x18||estado==0x28||estado==0x40){
		return 1;
		}else{
		return estado;
	}
	
	
}
uint8_t I2C_Master_Write_stemp(uint8_t dato){
	
	uint8_t estado;
	
	TWDR=dato; //carga texto
	TWCR=(1<<TWEN)|(1<<TWINT);//inicia el envio
	while(!(TWCR &(1<<TWINT)));
	estado=TWSR&0xF8;//verifica estado
	if(estado==0x18||estado==0x28||estado==0x40){
		return 1;
		}else{
		return estado;
	}
	
	
}

uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack){
	uint8_t estado;
	if(ack){
		TWCR|=(1<<TWEA);
		}else{
		TWCR&=~(1<<TWEA);
	}
	
	TWCR|=(1<<TWINT);//INICIA LECTURA
	while(!(TWCR&(1<<TWINT)));//ESPERA A TERMINAR DE LEER
	estado=TWSR&0xF8; //VERIFICAR ESTADO
	
	//verifica si el dato fue ledio con ACK o sin ACK
	if(estado==0x58||estado==0x50){
		*buffer=TWDR;
		return 1;
		}else{
		return estado;
	}
}



void I2C_Slave_Init(uint8_t adress){
	DDRC&=~((1<<DDC4)|(1<<DDC5));//ENTRADAS
	TWAR=adress<<1;//DIRECCIÓN
	TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWIE);//HABILITA INTERFAZ
}