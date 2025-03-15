/*
 * D2_Lab4_S_Bot.c
 *
 * Created: 2/18/2025 3:45:10 PM
 * Author : diego
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include <stdio.h>
#include <util/delay.h>
#include "I2C_S/I2C_S.h"

volatile uint8_t J1_Apachado = 0;
volatile uint8_t J2_Apachado = 0;
volatile uint8_t contador = 15;
volatile char buffer = 0;

#define slave2 0x20

void setup(void);

int main(void)
{
    /* Replace with your application code */
	setup();
    while (1) 
    {
		PORTD = (PORTD & 0x0F) | ((contador<<4)&0xF0);
    }
}

void setup(void){
	cli();
	DDRD |= 0xF0;
	PCMSK1 |= 0x03;
	PCICR |= (1 << PCIE1);
	DDRC = 0;
	I2C_Slave_Init(slave2);
	sei();
}

ISR(PCINT1_vect){
	if (!(PINC & (1 << PINC0))) // Decrementa
	{
		if (J2_Apachado == 0)
		{
			if (contador > 0)
			{
				contador -= 1;
			}
		}
		J2_Apachado = 1;
	}
	else {
		J2_Apachado = 0;
	}
	if (!(PINC & (1 << PINC1))) // Incrementa
	{
		if (J1_Apachado == 0)
		{
			if (contador < 15)
			{
				contador += 1;
			}
		}
		J1_Apachado = 1;
	}
	else {
		J1_Apachado = 0;
	}
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
		TWDR = contador; // Cargar el dato
		TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA); // Inicia el envio
		break;
		default:
		TWCR |= (1<<TWINT)|(1<<TWSTO);
	}
}