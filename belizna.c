/*
 * belizna.c
 *
 * Created: 26.11.2016 0:51:52
 * Author: Falcon
 */

#include <mega328p.h>

#include <delay.h>

#define LED PORTB.5
#define SENS PORTD.2
#define PREHEAT_TIME 20
#define COOLING_TIME 30
#define PROBES 20
#define ETALON_100 1000

// Declare your global variables here
unsigned char i;
unsigned int adc, dark;

// Standard Input/Output functions
#include <stdio.h>

// Voltage Reference: Int., cap. on AREF
#define ADC_VREF_TYPE ((1<<REFS1) | (1<<REFS0) | (0<<ADLAR))

// ADC interrupt service routine
interrupt [ADC_INT] void adc_isr(void)
{
unsigned int adc_data;
// Read the AD conversion result
ADCSRA|=(0<<ADSC);
LED=0;
SENS=0;
adc+=ADCW;
}

void main(void)
{
PORTB.5=0;
DDRB.5=1;

DDRD.2=1;
PORTD.2=0;

// Communication Parameters: 8 Data, 1 Stop, No Parity Asynchronous 9600
UCSR0B=(1<<TXEN0);
UCSR0C=(1<<UCSZ01) | (1<<UCSZ00);

UBRR0L=0x67;


// ADC Clock frequency: 1000,000 kHz  Reference: Int.
DIDR0=(1<<ADC5D) | (1<<ADC4D) | (1<<ADC3D) | (1<<ADC2D) | (1<<ADC1D) | (0<<ADC0D);
ADMUX=ADC_VREF_TYPE;
ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (1<<ADIE) | (1<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);
ADCSRB=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);


printf("Dark BaseLine Measuring...");
LED=0;
for(i=0; i<PROBES; i++) 
{
SENS=1;
delay_ms(PREHEAT_TIME);
ADCSRA|=(1<<ADSC);
delay_ms(5);
SENS=0;
dark+=ADCW;
delay_ms(COOLING_TIME);
}
dark/=PROBES;
printf("Dark Level = %i pt.", dark);
delay_ms(500);
LED=1;
printf("Whiteness Measuring...");
delay_ms(500);
// Global enable interrupts
#asm("sei")

while (1)
      {
      // Place your code here
      for (i=0; i<PROBES;i++)
       {
        LED=1;
        SENS=1;
        delay_ms(PREHEAT_TIME);
        ADCSRA|=(1<<ADSC);
        delay_ms(5);
        LED=0;
        SENS=0;
        delay_ms(COOLING_TIME);
       }
      adc/=PROBES;
      printf("%i %%", (int)((float)(adc-dark)/(float)(ETALON_100-dark)*100));
      adc=0;
      }
}