/*
 * Slave_Pulso.c
 *
 * Created: 2/27/2025 4:06:59 PM
 * Author : diego
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Interrupt_Pulse/Interrupt_Pulse.h"
#include "I2C_S/I2C_S.h"
volatile char buffer;

#define slave1 0x20


uint16_t rate[10]; 
volatile int N = 0;
int runningTotal = 0;                   // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
unsigned long lastBeatTime = 0;           // used to find IBI
uint16_t P = 530;                      // used to find peak in pulse wave, seeded
uint16_t T = 530;                     // used to find trough in pulse wave, seeded
uint16_t thresh = 530;                // used to find instant moment of heart beat, seeded
uint8_t amp = 100;                   // used to hold amplitude of pulse waveform, seeded
uint8_t firstBeat = 1;        // used to seed rate array so we startup with reasonable BPM
uint8_t secondBeat = 0;      // used to seed rate array so we startup with reasonable BPM
uint8_t BPM = 255;                   // Pulsaciones por minuto
uint16_t Signal;                // Entrada de datos del sensor de pulsos
int IBI = 600;             // tiempo entre pulsaciones
uint8_t Pulse = 0;     // Verdadero cuando la onda de pulsos es alta, falso cuando es Baja
uint8_t QS = 1;        // Verdadero cuando el Arduino Busca un pulso del Corazon
char cadena[5];
char *valor_BPM_t;
volatile char bufferRX;
volatile uint8_t bandera_lectura = 0;


void setup();
void writetxtUART(char* texto);

int main(void)
{
    /* Replace with your application code */
	setup();
	writetxtUART("Hola Mundo");
	
	
    while (1) 
    {
		
		//  find the peak and trough of the pulse wave
		if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
			if (Signal < T){                        // T is the trough
				T = Signal;                         // keep track of lowest point in pulse wave
			}
		}

		if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
			P = Signal;                             // P is the peak
		}                                        // keep track of highest point in pulse wave

		//  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
		// signal surges up in value every time there is a pulse
		if(firstBeat == 1){                         // if it's the first time we found a beat, if firstBeat == TRUE
			firstBeat = 0;                   // clear firstBeat flag
			secondBeat = 1;                   // set the second beat flag
		} else {
			if (N > 250){                                   // avoid high frequency noise
				//valor_BPM_t = itoa(BPM, cadena, 10);
				//writetxtUART(valor_BPM_t);
				//writetxtUART("\n");
				if ( (Signal > thresh) && (Pulse == 0) && (N > (IBI/5)*3) ){
					Pulse = 1;                               // set the Pulse flag when we think there is a pulse

					IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
					lastBeatTime = sampleCounter;               // keep track of time for next pulse

					if(secondBeat == 1){                        // if this is the second beat, if secondBeat == TRUE
						secondBeat = 0;                  // clear secondBeat flag
						for(int i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
							rate[i] = IBI;
						}
					}


					// keep a running total of the last 10 IBI values
						runningTotal = 0;                  // clear the runningTotal variable

						for(int i=0; i<=8; i++){                // shift data in the rate array
							rate[i] = rate[i+1];                  // and drop the oldest IBI value
							runningTotal += rate[i];              // add up the 9 oldest IBI values
						}

						rate[9] = IBI;                          // add the latest IBI to the rate array
						runningTotal += rate[9];                // add the latest IBI to runningTotal
						runningTotal /= 10;                     // average the last 10 IBI values
						BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
						QS = 1;                              // set Quantified Self flag
						// QS FLAG IS NOT CLEARED INSIDE THIS ISR
				
						}
			}
			if (Signal < thresh && Pulse == 1){   // when the values are going down, the beat is over

				Pulse = 0;                         // reset the Pulse flag so we can do it again
				amp = P - T;                           // get amplitude of the pulse wave
				thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
				P = thresh;                            // reset these for next time
				T = thresh;
			}

			if (N > 2500){                           // if 2.5 seconds go by without a beat
				thresh = 530;                          // set thresh default
				P = 530;                               // set P default
				T = 530;                               // set T default
				lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
				firstBeat = 1;                      // set these to avoid noise
				secondBeat = 0;                    // when we get the heartbeat back
			}
			
		}
		//valor_BPM_t = itoa(BPM, cadena, 10);
		//writetxtUART(valor_BPM_t);
		
		
    }
}

void writetxtUART(char* texto){
	uint8_t i;
	for (i=0;texto[i]!='\0';i++){
		while(!(UCSR0A&(1<<UDRE0)));//esperar hasta que el udre0 esté en 1
		UDR0=texto[i];//cuando i nulo se acaba
	}
}

void init_ADC(void){
	ADMUX = 0;
	//Vref = AVCC = 5V
	ADMUX |= (1<<REFS0);
	// Justificado a la derecha
	ADMUX &= ~(1<<ADLAR);
	
	ADCSRA = 0;
	// Habilitar ADC
	ADCSRA |= (1<<ADEN);
	// Sin Máscara de interrupción del ADC
	//ADCSRA |= (1<<ADIE);
	// Prescaler del ADC a 128
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	
	ADCSRB = 0;
	
	// El ADC está configurado para correr en modo Single Conversion
	//Iniciar primera conversión
	//ADCSRA |= (1<<ADSC);
}

void init_UART9600(void){
	
	// Configurar pines TX y RX
	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD1);
	
	//Configurar A modo Fast U2X0 = 1
	UCSR0A = 0;
	UCSR0A |= (1<<U2X0);
	
	//Configurar B Habilitar ISR RX
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	
	// Paso 4: Configurar C Frame: 8 bits datos, no paridad, 1 bit stop
	UCSR0C = 0;
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	
	// Baudrate = 9600
	UBRR0 = 207;
	
}

void setup(void){
	DDRD |= (1<<DDD2);
	cli();
	I2C_Slave_Init(slave1);
	interruptSetup();
	init_ADC();
	init_UART9600();
	sei();
}

ISR(TIMER2_COMPA_vect){                         // triggered when Timer2 counts to 124
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	Signal = (Signal & 0x0000) | (ADCL) | (ADCH << 8);
	sampleCounter += 2;                         // keep track of the time in mS with this variable
	N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise
}

/*ISR(ADC_vect){
	ADCSRA &= ~(1<<ADEN);
	
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
	ADCSRA |= (1<<ADIF);
}*/

ISR(USART_RX_vect){
	bandera_lectura = 1;
	bufferRX = UDR0;
}

ISR(TWI_vect){
	uint8_t state = TWSR & 0xFC;
	switch(state){
		case 0x60:
		case 0x70:
		TWCR |= (1<<TWINT);
		break;
		case 0x80:
		case 0x90:
		buffer = TWDR;
		TWCR |= (1<<TWINT); // Se limpia la bandera
		break;
		case 0xA8:
		case 0xB8:
		TWDR = BPM; // Cargar el dato
		TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA); // Inicia el envio
		break;
		default:
		TWCR |= (1<<TWINT)|(1<<TWSTO);
	}
}