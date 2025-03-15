/*
 * D2_Lab4_S_Pote.c
 *
 * Created: 2/13/2025 7:15:42 PM
 * Author : diego
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "I2C_S/I2C_S.h"

volatile uint8_t valor_pot0 = 0;
volatile char buffer;

#define slave1 0x10

void setup(void);

int main(void)
{
    /* Replace with your application code */
	setup();
    while (1) 
    {
    }
}


void init_ADC(void){
	ADMUX = 0;
	//Vref = AVCC = 5V
	ADMUX |= (1<<REFS0);
	// Justificado a la izquierda
	ADMUX |= (1<<ADLAR);
	
	ADCSRA = 0;
	// Habilitar ADC
	ADCSRA |= (1<<ADEN);
	// Máscara de interrupción del ADC
	ADCSRA |= (1<<ADIE);
	// Prescaler del ADC a 128
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	
	ADCSRB = 0;
	
	// El ADC está configurado para correr en modo Single Conversion
	//Iniciar primera conversión
	ADCSRA |= (1<<ADSC);
}

void setup(void){
	cli();
	DDRC = 0;
	I2C_Slave_Init(slave1);
	init_ADC();
	sei();
}

ISR(ADC_vect){
	ADCSRA &= ~(1<<ADEN);
	valor_pot0 = ADCH;
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
	ADCSRA |= (1<<ADIF);
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
			TWDR = valor_pot0; // Cargar el dato
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA); // Inicia el envio
			break;
		default:
			TWCR |= (1<<TWINT)|(1<<TWSTO);
	}
}