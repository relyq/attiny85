/*
 * Author : relyq
 *
 * programa para probar shift register
 *
 * PB0 - SER   - 595 pin 14
 * PB1 - SRCLK - 595 pin 11
 * PB2 - RCLK  - 595 pin 12
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define SER_PORT PORTB
#define SRCLK_PORT PORTB
#define RCLK_PORT PORTB

#define SER_PIN PB0
#define SRCLK_PIN PB1
#define RCLK_PIN PB2

#define SET_SER() \
  { SER_PORT |= (1 << SER_PIN); }
#define CLEAR_SER() \
  { SER_PORT &= ~(1 << SER_PIN); }
#define SHIFT_DATA()                 \
  {                                  \
    SRCLK_PORT |= (1 << SRCLK_PIN);  \
    _delay_ms(1);                    \
    SRCLK_PORT &= ~(1 << SRCLK_PIN); \
  }
#define STORE_DATA()               \
  {                                \
    RCLK_PORT |= (1 << RCLK_PIN);  \
    _delay_ms(1);                  \
    RCLK_PORT &= ~(1 << RCLK_PIN); \
  }

char counter;

ISR(TIMER0_OVF_vect){
  counter++;
}

int main(void) {
  DDRB = (1 << RCLK_PIN) | (1 << SRCLK_PIN) | (1 << SER_PIN);
  PORTB &= ~(1 << SRCLK_PIN) | ~(1 << RCLK_PIN);

  TCCR0B = (1 << CS02) | (1 << CS00);  // clk/1024
  TIMSK = (1 << TOIE0);

  SET_SER();

  sei();

  while (1) {
    if(counter == 15){
      SHIFT_DATA();
      _delay_ms(1);
      STORE_DATA();
      counter = 0;
    }
  }
}