/*
 * Master_pj1.c
 *
 * Created: 2/27/2025 3:58:24 PM
 * Author : njfg0
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "I2C/I2C.h"
#include "lcd/lcd.h"
#include "com_uart/com_uart.h"

#define temp_adress 0x38
#define slave1 0x10 //ultrasonico
#define slave2 0x20 //pulso cardiaco


uint8_t bufferI2C;
float temperatura;
uint8_t temp_f;
char * temperatura_t;
uint8_t flag_sT=0;
uint8_t temp;
uint8_t data_Ttemp[6];
uint8_t estado_sT;
uint8_t estado_us;
char cadena1_u[5];
char cadena1_d[5];
uint8_t us_sensor_data[3];
uint16_t distancia=0;
uint8_t s_pulso=0;
uint8_t turbina=0;
long delay_uart = 0;
volatile char buffRX;
uint8_t leer_uart = 0;
char *wordRX;
char *inicio_wordRX;
char cadena[3];
uint8_t cambiar_SAdafruit = 0;
uint8_t estado_adafruit_dc = 0;

//iconos para LCD
uint8_t temp_icon[8]={
	0b00100,0b01010,0b01010,0b01010,0b01110,0b01110,0b01110,0b00100
};

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

void escribir_esp_servo(uint8_t estado){
	I2C_Master_Start();
	temp = I2C_Master_Write((slave1 << 1) | 0);//Modo escribir
	I2C_Master_Write('S');  // Enviar primer byte ('S')
	I2C_Master_Write(estado?'1':'0');  // Enviar segundo byte 1
	I2C_Master_Stop();
}

float temp_converter(uint8_t *data){
	uint32_t rawTemp = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
	return (rawTemp * 200.0) / 1048576.0 - 50;
}

int main(void)
{
	cli();
	I2C_Master_Init(100000,1); // Inicializa I2C a 100KHz
	initLCD8b();
	initUART9600();
	DDRB |= (1<<DDB5);
	PORTB |= (1<<PORTB5);
	sei();
	
	// Enviar comando de inicialización (si necesario)
	I2C_Master_Start();
	temp = I2C_Master_Write((temp_adress << 1) | 0);
	I2C_Master_Write(0xE1);
	I2C_Master_Stop();
	_delay_ms(10);

	//cargar icono
	LCD_Set_Icon(0,temp_icon);

	LCD_Set_Cursor(0,1);
	LCD_write_char(0);
	wordRX = cadena;
	inicio_wordRX = wordRX;

	while (1)
	{
		
		// Enviar comando de medición
		I2C_Master_Start();
		temp = I2C_Master_Write((temp_adress << 1) | 0);
		if (temp != 1) {
			I2C_Master_Stop();
			continue;
		}

		I2C_Master_Write(0xAC);
		I2C_Master_Write(0x33);
		I2C_Master_Write(0x00);
		I2C_Master_Stop();

		// Esperar hasta que la medición esté lista
		uint8_t estado = 0;
		do {
			I2C_Master_Start();
			temp = I2C_Master_Write((temp_adress << 1) | 1);
			I2C_Master_Read(&estado, 1);
			I2C_Master_Stop();
		} while (estado & 0x80);

		// Leer datos
		I2C_Master_Start();
		temp = I2C_Master_Write((temp_adress << 1) | 1);
		for (uint8_t i = 0; i < 6; i++) {
			estado_sT = I2C_Master_Read(&data_Ttemp[i], (i < 5)); // ACK en los primeros 5
		}
		I2C_Master_Stop();

		// Convertir temperatura
		temperatura = temp_converter(data_Ttemp);
		temp_f = (int)temperatura;
		
		if (temp_f>28){
			PORTB |= (1 << PORTB4);//enciende turbina
			turbina=1;
		}else{
			PORTB &= ~(1 << PORTB4);//apaga turbina
			turbina=0;
		}

		// Mostrar en LCD
		LCD_Set_Cursor(1,1);
		char salida[15];
		sprintf(salida, "%2d'C  ", temp_f);
		LCD_write_String(salida);

				
		//COMIENZA A LEER EL SENSOR ULTRASONICO
		I2C_Master_Start();
		temp = I2C_Master_Write((slave1 << 1) | 1);
		for(uint8_t i=0;i<3;i++){
			estado_us=I2C_Master_Read(&us_sensor_data[i],i<2);
		}
		I2C_Master_Stop();
		
		distancia=(us_sensor_data[0]<<8)|us_sensor_data[1];
		uint8_t estado_servos=us_sensor_data[2];
		
		LCD_Set_Cursor(0,2);
		LCD_write_String("D:");
		LCD_Set_Cursor(1,2);
		char vect_salida[16];
		sprintf(vect_salida, "%dcm ", distancia);
		LCD_write_String(vect_salida);
		_delay_ms(70);
		
		//COMIENZA A LEER EL SENSOR DE PULSO
		I2C_Master_Start();
		temp = I2C_Master_Write((slave2 << 1) | 1);
		PORTB &= ~(1<<PORTB5);
		if (temp!=1){
			I2C_Master_Stop();
		}else{
			TWCR=(1<<TWINT)|(1<<TWEN);
			while(!(TWCR&(1<<TWINT)));
			s_pulso=TWDR;
			I2C_Master_Stop();
		}
		LCD_Set_Cursor(6,2);
		LCD_write_String("P:");
		LCD_Set_Cursor(9,2);
		char salida_pulso[16];
		sprintf(salida_pulso,"%d bpm  ",s_pulso);
		LCD_write_String(salida_pulso);
		
		uint8_t LS_dist=distancia%100;
		char mensaje_esp32[10];
		uint8_t temp_c = temp_f;
		
		if (delay_uart > 50){
			delay_uart = 0;
			sprintf(mensaje_esp32,"<%c%c%c%d%d>",LS_dist,temp_c,s_pulso, turbina, estado_servos);
			writetxtUART(mensaje_esp32);
		}
		delay_uart += 1;
		
		if (leer_uart == 1){
			leer_uart = 0;
			estado_adafruit_dc = cadena[0] - '0';
			cambiar_SAdafruit = cadena[1] - '0';
		}
		
		//AQUI MANDA DATOS SI EL ESP LE MANDA UN 1 PARA SERVOS
		//uint8_t cambiar_SAdafruit=0; esta es la variable del led en adafruit incluir una bandera si en adafruit cambio o algo así
		if (estado_servos != cambiar_SAdafruit){
			escribir_esp_servo(cambiar_SAdafruit);
		}
		//SE LEE VARIABLE DE ADAFRUIT PARA EL MOTOR DC TURBINA
		if (estado_adafruit_dc != turbina){
			if (estado_adafruit_dc==1){
				PORTB |= (1 << PORTB4);//enciende turbina
				turbina = 1;
				}else if(estado_adafruit_dc==0){
				PORTB &= ~(1 << PORTB4);//apaga turbina
				turbina = 0;
			}
		}
	}
}

ISR(USART_RX_vect){
	buffRX=UDR0;
	if ((buffRX == '\n')||(buffRX == '\r')) { //(buffRX==0)||
		leer_uart = 1;
		*wordRX = '\0';
		wordRX++;
		wordRX = inicio_wordRX;
		} else {
		*wordRX = buffRX;
		wordRX++;
	}
}