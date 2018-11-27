
/*
  Quick and dirty functions that make serial communications work.
*/

#include <avr/io.h>
#include "usart.h"
#include <util/setbaud.h>

void initUSART(void) {                                /* requires BAUD */
  UBRR0H = UBRRH_VALUE;                        /* defined in setbaud.h */
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
  UCSR0A &= ~(1 << U2X0);
#endif
                                  /* Enable USART transmitter/receiver */
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
 
  // UCSRC - USART control register
  // bit 7-6      sync/ascyn 00 = async,  01 = sync ( 00 = async)
  // bit 5-4      parity 00 = disabled
  // bit 3        stop bits 0 = 1 bit  1 = 2 bits ( 0 = 1 bit)
  // bit 2-1      frame length 11 = 8
  // bit 0        clock polarity = 0
  UCSR0C  = 0b00000110;
 
  // Enable uart, tx and rx
  UDR0 = 0x00; // send an empty byte to indicate powerup.
}

