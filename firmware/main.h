#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

#include "cube.h"
#include "pin_defines.h"

#define DATA_BUS PORTA
#define LAYER_SELECT PORTC
#define LATCH_ADDR PORTB
#define LATCH_MASK 0x07
#define LATCH_MASK_INV 0xf8
#define OE_PORT PORTB
#define OE_MASK 0x08

void initLEDs (void);
void initButtons (void);
void initTimer2 (void);
void bootmsg (void);

volatile unsigned char current_layer;
volatile unsigned char pgm_mode;
void uart(void);
unsigned int bootwait (void);
#endif

