/*
 * t85_twi.c
 *
 * Created: 14/08/2020 05:02:15
 * Author : relyq
 *
 */

#define F_CPU 8000000UL

#define SET_USI_START_CONDITION_MODE()                                      \
  {                                                                         \
    USICR = (1 << USISIE) | (0 << USIOIE) | (1 << USIWM1) | (0 << USIWM0) | \
            (1 << USICS1) | (0 << USICS0) | (0 << USICLK) | (0 << USITC);   \
    USISR = (0 << USISIF) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);    \
  }

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

typedef enum { USI_SLAVE_CHECK_ADDRESS, USI_SLAVE_SEND_DATA } TWIState;

void initTWI();

static unsigned char slaveAddress;
static TWIState twi_state;

int main(void) {
  initTWI(0x01);
  while (1) {
  }
}

void initTWI(unsigned char address) {
  slaveAddress = address;
  // setup USI
  USICR = 0x00;
  USICR |= (1 << USIWM1);  // set USI to three wire mode
  USICR |= (1 << USICS1);
  DDRB &= (0 << PB0);
  DDRB |= (1 << PB2);
  PORTB |= (1 << PB0) | (1 << PB2);
  USISR = 0xF0;
  // setup interrupt
  USICR |= (1 << USISIE);  // set USI start condition int enable
  USICR &= (0 << USIOIE);  // set USI counter overflow enable
  sei();
}
