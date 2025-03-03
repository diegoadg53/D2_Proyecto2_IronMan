/*
 * Interrupt_Pulse.c
 *
 * Created: 2/27/2025 4:09:48 PM
 *  Author: diego
 */ 

#include "Interrupt_Pulse.h"

void interruptSetup(){
	// Initializes Timer2 to throw an interrupt every 2mS.
	TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
	TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER
	OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
	TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
}

// THIS IS THE TIMER 2 INTERRUPT SERVICE ROUTINE.
// Timer 2 makes sure that we take a reading every 2 miliseconds


