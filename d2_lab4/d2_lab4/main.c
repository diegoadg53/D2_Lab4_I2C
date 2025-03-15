/*
 * d2_lab4.c
 *
 * Created: 2/13/2025 7:10:11 PM
 * Author : njfg0
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "I2C/I2C.h"
#include "lcd/lcd.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define slave1 0x10
#define slave2 0x20

uint8_t direccion;
uint8_t temp;
uint8_t bufferI2C;
uint8_t valorpot=0;
uint8_t valorcont=0;

char cadena1_u[5];
char cadena1_d[5];
char cadena1_c[5];
char cadena2_u[5];
char cadena2_d[5];
char *valorpot_t;
char *valorcont_t;

//void initPorts(void);


int main(void)
{	
	cli();
	I2C_Master_Init(100000,1); //iniciar como master frec 100KHz, preescaler 1
	initLCD8b();
	DDRB|=(1<<DDB4);
	sei();
	
	LCD_Set_Cursor(1,1);
	LCD_write_String("S1:");
	LCD_Set_Cursor(8,1);
	LCD_write_String("S2:");
	
	//initPorts();
    /* Replace with your application code */
    while (1) 
    {	
		PORTB|=(1<<PORTB4);
		//le habla al slave del potenciometro
		I2C_Master_Start();
		bufferI2C=slave1<<1|0b00000001;
		temp=I2C_Master_Write(bufferI2C);
		if (temp!=1){
			I2C_Master_Stop();
			PORTB&=~(1<<PORTB4);
		}else{
		
			TWCR=(1<<TWINT)|(1<<TWEN);
			while(!(TWCR&(1<<TWINT)));
			valorpot=TWDR;
			I2C_Master_Stop();
			//_delay_ms(500);
			PORTB&=~(1<<PORTB4);
		}
		//_delay_ms(500);
		
		
		//le habla al slave del contador de leds
		I2C_Master_Start();
		bufferI2C=slave2<<1|0b00000001;
		temp=I2C_Master_Write(bufferI2C);
		if (temp!=1){
			I2C_Master_Stop();
		} else {
		
			TWCR=(1<<TWINT)|(1<<TWEN);
			while(!(TWCR&(1<<TWINT)));
			valorcont=TWDR;
			I2C_Master_Stop();
		}
		
		
		LCD_Set_Cursor(0,2);
		char salida[15];
		strcpy(salida, " ");
		if (valorpot<10) {
			valorpot_t=itoa(valorpot, cadena1_u, 10);
			strcat(salida, valorpot_t);
			strcat(salida, "  ");
			} else if (valorpot<100){
			valorpot_t = itoa(valorpot, cadena1_c, 10);
			strcat(salida, valorpot_t);
			strcat(salida, " ");
			} else {
			valorpot_t=itoa(valorpot,cadena1_d,10);
			strcat(salida, valorpot_t);
		}
		
		strcat(salida, "    ");
		
		//valorcont_t=itoa(valorcont,cadena2,10);
		if (valorcont<10){
			valorcont_t=itoa(valorcont, cadena2_u, 10);
			strcat(salida, valorcont_t);
			strcat(salida, " ");
		} else {
			valorcont_t=itoa(valorcont, cadena2_d, 10);
			strcat(salida, valorcont_t);
		}
		
		LCD_write_String(salida);
		cadena1_u[0] = '\0';
		cadena1_d[0] = '\0';
		cadena1_c[0] = '\0';
		cadena2_d[0] = '\0';
		cadena2_u[0] = '\0';
		
		//valorpot_t[0]='\0';
		//LCD_Set_Cursor(8,2);
		//LCD_write_String(valorcont_t);
    }
}

