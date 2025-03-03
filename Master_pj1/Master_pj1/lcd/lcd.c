/*
 * lcd.c
 *
 * Created: 2/27/2025 8:30:29 PM
 *  Author: njfg0
 */ 
#include "lcd.h"
void initLCD8b(void){
	DDRD|=(1<<DDD7)|(1<<DDD6)|(1<<DDD5)|(1<<DDD4)|(1<<DDD3)|(1<<DDD2);
	DDRB|=(1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3);
	PORTB=0;
	PORTD=0;
	
	port_disp(0x00);
	_delay_ms(20);
	
	//function set
	comandos_LCD(0b00111000);
	_delay_us(100);
	
	//display on/off
	comandos_LCD(0b1100);
	_delay_us(100);
	
	//Entry mode
	comandos_LCD(0b0110);
	_delay_us(100);
	
	//clear display
	comandos_LCD(0b0001);
}

void comandos_LCD(char a){
	//RS=0;
	PORTD&=~(1<<PORTD2);
	port_disp(a);
	//enable=1;
	PORTD|=(1<<PORTD3);
	_delay_ms(4);
	//E=0;
	PORTD&=~(1<<PORTD3);
}

void port_disp(char a){
	if (a&1)
	PORTD|=(1<<PORTD4);
	else
	PORTD&=~(1<<PORTD4);
	
	if (a&2)
	PORTD|=(1<<PORTD5);
	else
	PORTD&=~(1<<PORTD5);
	
	if (a&4)
	PORTD|=(1<<PORTD6);
	else
	PORTD&=~(1<<PORTD6);
	
	if (a&8)
	PORTD|=(1<<PORTD7);
	else
	PORTD&=~(1<<PORTD7);
	
	if (a&16)
	PORTB|=(1<<PORTB0);
	else
	PORTB&=~(1<<PORTB0);
	
	if (a&32)
	PORTB|=(1<<PORTB1);
	else
	PORTB&=~(1<<PORTB1);
	
	if (a&64)
	PORTB|=(1<<PORTB2);
	else
	PORTB&=~(1<<PORTB2);
	
	if (a&128)
	PORTB|=(1<<PORTB3);
	else
	PORTB&=~(1<<PORTB3);
}

void LCD_write_char(char d){
	//manda y escribe el mensaje
	//RS=1
	PORTD|=(1<<PORTD2);
	port_disp(d);
	//E=1
	PORTD|=(1<<PORTD3);
	_delay_ms(4);
	//E=0
	PORTD&=~(1<<PORTD3);
	
}

void LCD_write_String(char *a){
	int i;
	for (i=0;a[i]!='\0';i++)
	LCD_write_char(a[i]);
}


void LCD_Set_Cursor(char c, char f){
	char temp;
	if (f==1){
		temp=0x80+c;
		comandos_LCD(temp);
		}else if(f==2){
		temp=0xC0+c;
		comandos_LCD(temp);
	}
}

void LCD_Set_Icon(uint8_t adress, uint8_t *icon){
	adress&=0x7;
	comandos_LCD(0x40+(adress*8));
	int i;
	for (i=0;i<8;i++){
		LCD_write_char(icon[i]);
	}
}
