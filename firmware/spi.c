
#include <avr/io.h>
#include "spi.h"
#include "pin_defines.h"

void initSPI(void) {
  SPI_SS_DDR |= (1 << SPI_SS);                        /* set SS output */
  SPI_SS_PORT |= (1 << SPI_SS);       /* start off not selected (high) */

  SPI_MOSI_DDR |= (1 << SPI_MOSI);                   /* output on MOSI */
  SPI_MISO_PORT |= (1 << SPI_MISO);                  /* pullup on MISO */
  SPI_SCK_DDR |= (1 << SPI_SCK);                      /* output on SCK */

  /* Don't have to set phase, polarity b/c
   * default works with 74HC595 chips */
  /* SPR1 = 0, SPR0=0 default Oscillator Frequency fosc/4 */
  SPCR |= (1 << MSTR);                                  /* clockmaster */
  SPCR |= (1 << SPE);                                        /* enable */
  SPSR |= (1 << SPI2X);
}

void SPI_tradeByte(uint8_t byte) {
  SPDR = byte;                       /* SPI starts sending immediately */
  loop_until_bit_is_set(SPSR, SPIF);                /* wait until done */
                                /* SPDR now contains the received byte 
                                   if MISO is connected                */
}