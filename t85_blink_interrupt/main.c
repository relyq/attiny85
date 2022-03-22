/*
 * t85blink.cpp
 *
 * Created: 11/08/2020 04:57:14
 * Author : relyq
 */

#include <avr/interrupt.h>
#include <avr/io.h>

ISR(TIMER0_OVF_vect) { PORTB ^= 1 << PB1; }

int main(void) {
  DDRB = 1 << PB1;                /* PB1 output */
  TCCR0B = 1 << CS02 | 1 << CS00; /* clk/1024 */
  TIMSK = 1 << TOIE0;

  sei();

  while (1) {
    MCUCR |= (1 << SE) | (1 << SM1);
  }
}