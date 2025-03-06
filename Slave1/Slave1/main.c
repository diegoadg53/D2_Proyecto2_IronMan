/*
 * Slave1.c
 *
 * Created: 3/1/2025 1:02:35 AM
 * Author : njfg0
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>

#include "com_uart/com_uart.h"
#include "I2C/I2C.h"

#define TRIG_PIN PB5
#define ECHO_PIN PD2
#define slave1 0x10
#define buzzer PB4
#define SERVO1_PIN PD6
#define SERVO2_PIN PD5


uint8_t Pulse_Servo=0;
uint8_t estado_servos=0; //0 cerrado, 1 abierto

volatile uint16_t pulso_echo=0;
volatile uint16_t distancia=0;
uint8_t sensor_flag=0;
volatile char buffer = 0;

//Timer 0
void PWM0_init(){
	TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1); //modo fast timero 0
	TCCR0B = (1 << CS01) | (1 << CS00); //preescaler de 64
}

void PWM2_init(){
	TCCR2A = (1 << WGM21) | (1 << WGM20) | (1 << COM2A1); //modo fast timero 0
	TCCR2A = (1 << CS21) | (1 << CS20); //preescaler de 64
}

//Timer 1 interrupción cada 0.5us
void Timer1_init() {
	TCCR1A = 0;              
	TCCR1B = (1 << CS11);    //  0.5 µs
	TCNT1 = 0;               // Reiniciar contador
}


uint8_t calcular_pwm0(int angulo){
	int pulso_min=31; //1ms*256/20ms
	int pulso_max=62; //2ms*256/20ms
	Pulse_Servo=(((angulo)*(pulso_max-pulso_min))/(90+pulso_min));
	return Pulse_Servo;
}


// Función para medir distancia
void medir_distancia() {
	PORTB &= ~(1 << TRIG_PIN);  
	_delay_us(2);
	PORTB |= (1 << TRIG_PIN);  
	_delay_us(10);
	PORTB &= ~(1 << TRIG_PIN);

	// Esperar a que ECHO pase a HIGH
	while (!(PIND & (1 << ECHO_PIN)));
	TCNT1 = 0;  // Reiniciar Timer1
	while (PIND & (1 << ECHO_PIN));  

	// Guardar el tiempo medido
	pulso_echo = TCNT1;
	distancia = (pulso_echo / 2) / 58;  // Convertir a cm
	//sensor_flag=0;
}



int main(void)
{
	 cli();  

	 DDRB |= (1 << TRIG_PIN);  // TRIG como salida
	 DDRD |= (1 << SERVO1_PIN);  // servo como salida
	 DDRD |= (1 << SERVO2_PIN);  // servo como salida
	 DDRB |= (1 << buzzer);  // buzzer como salida
	 DDRD &= ~(1 << ECHO_PIN); // ECHO como entrada
	 Timer1_init();
	 PWM0_init();
	 
	 I2C_Slave_Init(slave1);//SENSOR ULTRASONICO
	 initUART9600(); // Inicializar UART

	 sei();  // Habilitar interrupciones globales
	 

	 char vect_salida[16];
	 OCR0A=calcular_pwm0(0);
	 OCR0B=calcular_pwm0(45);
	 
	 while (1) {
		 medir_distancia(); 
		 _delay_ms(60);  
		uint8_t distancia_tempo=distancia;
		if (distancia_tempo<8){
			//for(uint8_t i=0; i<3;i++){
				PORTB|=(1<<buzzer);
				OCR0A=calcular_pwm0(45);
				OCR0B=calcular_pwm0(0);
				estado_servos=1;//abierto
				_delay_ms(5);
			//}
		} else {
			PORTB&=~(1<<buzzer);
			OCR0A=calcular_pwm0(0);
			OCR0B=calcular_pwm0(45);
			estado_servos=0; //cerrado
			_delay_ms(5);
		}
	
			
		 sprintf(vect_salida, "Dist: %d cm\n", distancia);
		// writetxtUART(vect_salida);
	 }
}

//Interrupción del I2C
ISR(TWI_vect){
	uint8_t state = TWSR & 0xFC;
	static uint8_t comando = 0;
	
	switch(state){
		case 0x60:
		case 0x70:
		comando = 0;
		TWCR |= (1<<TWINT);
		break;
		case 0x80:
		case 0x90:
		//AQUI LEE LA INFORMACIÓN DE ADAFRUIT DESDE EL MASTER, MANDA UN S1 O UN S2
		if (comando == 0) {  // Primer byte recibido
			buffer = TWDR;  // Guardamos el primer byte
			if (buffer == 'S') {  // Si es 'S', esperamos el siguiente byte
				comando = 1;
			}
			} else if (comando == 1) {  // Segundo byte recibido (valor del comando)
			if (TWDR == '1') {
				// Activar servos
				OCR0A = calcular_pwm0(0);
				OCR0B = calcular_pwm0(45);
				estado_servos = 0;
				} else if (TWDR == '0') {
				// Desactivar servos
				OCR0A = calcular_pwm0(45);
				OCR0B = calcular_pwm0(0);
				estado_servos = 1;
			}
			comando = 0;  // Reiniciar estado del comando
		}
		TWCR |= (1 << TWINT);  // Limpiar la bandera
		break;
		case 0xA8:
		case 0xB8:
		if (sensor_flag==0){
			TWDR = (distancia>>8); // Cargar alto byte
			sensor_flag=1;
		}else if(sensor_flag==1){
			TWDR = (distancia&0xFF); // Cargar bajo byte
			sensor_flag=2;
		}else if(sensor_flag==2){
			TWDR=estado_servos;
			sensor_flag=0;
		}
		TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA); // Inicia el envio
		break;
		default:
		TWCR |= (1<<TWINT)|(1<<TWSTO);
	}
}