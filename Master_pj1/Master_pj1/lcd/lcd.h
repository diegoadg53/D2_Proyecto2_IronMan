/*
 * lcd.h
 *
 * Created: 2/27/2025 8:30:21 PM
 *  Author: njfg0
 */ 


#ifndef LCD_H_
#define LCD_H_
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

#define E (1<<PORTD3)
#define RS (1<<PORTD2)
//inicializar el lcd en 4 bits
void initLCD8b(void);
//escribir un valor en el puerto de salida
void port_disp(char a);
//comandos
void comandos_LCD(char a);
//funcion para enviar u caracter
void LCD_write_char(char d);
//funcion para iniciar el cursor;
void LCD_Set_Cursor(char c,char f);
//funcion escirbir cadena de caracteres
void LCD_write_String(char *a);
//setear pantalla
void set_lcd_value(char vp1,char vp2);
//poner un icono en la pantalla
void LCD_Set_Icon(uint8_t adress, uint8_t *icon);


#endif /* LCD_H_ */