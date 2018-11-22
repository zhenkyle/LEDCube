#ifndef PORTDEF_H
#define PORTDEF_H

// buttons

#define BUTTON_PORT             PORTD
#define BUTTON_PIN              PIND
#define BUTTON_DDR              DDRD

#define MAIN_BTN                PD2
#define UART_BTN                PD3


// leds 
#define LED_PORT                PORTD
#define LED_PIN                 PIND
#define LED_DDR                 DDRD

#define LED_PGM                 PD4
#define LED_RED                 PD5
#define LED_GREEN               PD6


// Latch Pin connected to ST_CP of 74HC595
#define LATCH                   PB0
#define LATCH_PORT              PORTB
#define LATCH_PIN               PINB   // actually, latch pin is not used in input mode, define it anyway
#define LATCH_DDR               DDRB

// SPI serial mode defines

#define SPI_SS                  PB2
#define SPI_SS_PORT             PORTB
#define SPI_SS_PIN              PINB
#define SPI_SS_DDR              DDRB

// Data Pin - MOSI connected to DS of 74HC595
#define SPI_MOSI                PB3
#define SPI_MOSI_PORT           PORTB
#define SPI_MOSI_PIN            PINB
#define SPI_MOSI_DDR            DDRB

// Don't use this for 74HC595
#define SPI_MISO                PB4
#define SPI_MISO_PORT           PORTB
#define SPI_MISO_PIN            PINB
#define SPI_MISO_DDR            DDRB

// Clock Pin - SCK connected to SH_CP of 74HC595
#define SPI_SCK                 PB5
#define SPI_SCK_PORT            PORTB
#define SPI_SCK_PIN             PINB
#define SPI_SCK_DDR             DDRB

#endif
