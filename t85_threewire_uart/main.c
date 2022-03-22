/*
 * t85_threewire_uart.c
 *
 * Created: 13/08/2020 11:09:15
 * Author : relyq
 *
 * voy a intentar implementar implementar una interfaz UART half-duplex con la
 * USI
 */

#define F_CPU 8000000UL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

typedef enum {
  IDLE,
  TRANSMIT,
  TRANSMIT_STOPBIT,
  RECEIVE,
  DATA_PENDING
} AsynchronousStates;
unsigned char reverse(unsigned char b);
void initUart();
void putCharUart(unsigned char byte);

volatile static AsynchronousStates UartState;

ISR(TIMER0_COMPA_vect) {
  USICR |= (1 << USITC);
}

ISR(USI_OVF_vect) {
  TCCR0B &= ~(1 << CS01);   // reset prescaler
  TCNT0 = 0x00;             // reset timer0
  TIMSK &= ~(1 << OCIE0A);  // disable interrupt
  USISR &= ~(1 << USIOIF);

  PORTB |= (1 << PB1);  // stop bit

  UartState = IDLE;
}

int main(void) {
  initUart();
  while (1) {
    putCharUart('a');
    _delay_ms(1000);
  }
}

unsigned char reverseByte(unsigned char b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

void initUart() {
  // setup USI
  USICR = 0x00;
  USICR |= (1 << USIWM0);  // set USI to three wire mode
  USICR |= (1 << USICLK);
  DDRB |= (1 << PB1);
  DDRB |= (1 << PB2);
  PORTB |= (1 << PB1);     // set TX to IDLE
  // setup timer0
  TCCR0A = 0x00;
  TCCR0B = 0x00;
  TCCR0A |= (1 << WGM01);  // timer0 on CTC mode
  TCCR0B |= (1 << CS01);   // timer0 clk/8
  OCR0A = 103;
  // setup interrupt
  USICR |= (1 << USIOIE);  // set USI counter overflow enable
  sei();

  UartState = IDLE;
}

void putCharUart(unsigned char byte) {
  while (UartState != IDLE)
    ;
  UartState = TRANSMIT;

  DDRB |= (1 << PB1);

  byte = reverseByte(byte);
  USIDR = byte;

  TIFR &= ~(1 << OCF0A);   // clear compare interrupt flag
  TCCR0B &= ~(1 << CS01);  // reset prescaler
  TCNT0 = 0x00;            // reset timer0
  TCCR0B |= (1 << CS01);   // set prescaler
  OCR0A = 103;

  PORTB &= ~(1 << PB1);  // clear TX - frame start

  TIMSK |= (1 << OCIE0A);  // enable interrupt
}