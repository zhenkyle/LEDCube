/*
 * Code to controll an 8x8x8 ledcube using avr
 * http://www.instructables.com/id/Led-Cube-8x8x8/
 * See lisence.txt for lisence.
 */
#include "main.h"
#include "effect.h"
#include "launch_effect.h"
#include "draw.h"
#include "spi.h"
#include "usart.h"
#include <string.h>

// Main loop
// the AVR enters this function at boot time
int main (void)
{
	// Inits LEDS
	initLEDs();
	// Inits Buttons
	initButtons();
	// Inits Timer2
	initTimer2();
  	// Inits SPI 
  	initSPI();
	// Initial Latch
	LATCH_DDR |= (1 << LATCH);
	// Inits USART
  	initUSART();
        // This variable specifies which layer is currently being drawn by the
	// cube interrupt routine. We assign a value to it to make sure it's not >7.
	current_layer = 0;	

  	// Clear Cube
  	clearCube();

	int i;
	int j;

	// Boot wait
	i = bootwait();

	// Turn off LEDs
	LED_PORT &= ~(1 << LED_RED);
	LED_PORT &= ~(1 << LED_GREEN);

	// Enable interrupts
	// This starts the routine that draws the cube content
	sei();

	// Result for bootwait() is 2:
	// Go to uart mode. this function loops forever.
	if (i == 2)
	{
		uart();
	}

	// Result of bootwait() is something other than 2:
	// Do awesome effects. Loop forever.
	while (1)
	{
            if (i==1) {
		// Show the effects in a predefined order
		for (j=0; j < EFFECTS_TOTAL; j++)
		{
			LED_PORT ^= (1 << LED_PGM);
			launch_effect(j);
		}
	    } else {
		// Show the effects in a random order.
		launch_effect(rand()%EFFECTS_TOTAL);
	    }
	}

}

/*
 * Multiplexer/framebuffer routine
 * This function is called by an interrupt generated by timer 2.
 * Every time it runs, it does the following:
 * 1) Disable the output for the multiplexer array.
 * 2) Turn of all layers.
 * 3) Load the current layer from the cube buffer onto the
 *    multiplexer array.
 * 4) Enable output from the multiplexer array.
 * 5) Turn on the current layer.
 * 6) Increment the current_layer variable, so the next layer is
 *    drawn the next time this function runs.
*/

ISR(TIMER2_COMPA_vect)
{
	updateCube();
}

void updateCube()
{
	int i;

	// take the Latch low so
	// the LEDs don't change while you're sending in bits:
	LATCH_PORT &= ~ (1 << LATCH);

	// shift out the bits:
	SPI_tradeByte(1 << current_layer);

	for (i=0; i < 8; i++)
		SPI_tradeByte(cube[current_layer][i]);

	//take the latch pin high so the LEDs will light up:
	LATCH_PORT |= (1 << LATCH);

	current_layer++;
	if (current_layer == 8)
		current_layer = 0;
}

void clearCube()
{
	int i;
  	memset(cube,0x00,64);
	for (i=0; i<8; i++) {
		updateCube();
	}
}

void initLEDs()
{
	LED_DDR |= (1 << LED_GREEN);
	LED_DDR |= (1 << LED_RED);
	LED_DDR |= (1 << LED_PGM); // for debug
}

void initButtons()
{
	BUTTON_DDR &= ~(1 << MAIN_BTN);  // makes double-sure we're in input mode
	BUTTON_PORT |= (1 << MAIN_BTN);  // enables pull-up resistor
}

void initTimer2()
{
	// Timer 2
	// Frame buffer interrupt = clock / prescaler / (OCR2A + 1)
	// 16000000/256/11 = 5681.82 interrupts per second
	// frame per second = Frame buffer interrup / 8
	// 5681.82/8 = 710.23 frames per second
  
    // Reset any PWM configuration that the arduino may have set up automagically!
	TCCR2A = 0x00;
	TCCR2B = 0x00;

	TCCR2A |= (0x01 << WGM21); // CTC mode. clear counter on TCNT2 == OCR2A
	OCR2A = 10; // Interrupt every 2816 cpu cycle ( prescaler * (OCR2A+1) = 256 * 11)
	TCNT2 = 0x00; // start counting at 0
	TCCR2B |= (0x01 << CS22) | (0x01 << CS21); // Start the clock with a 256 prescaler

	TIMSK2 |= (0x01 << OCIE2A);
}

// Boot wait function
// return 1 for automatic mode, return 2 for UART mode
// 1) if it is in automatic mode, wait for 5 seconds and return.
//    Delay starting up any interrupts for 5 seconds, as drawing the cube causes a lot
//    noise that can confuse the ISP programme.
//    Note in this mode, the LEDCube generate the same "random" sequence every time.
// 2) if it is in UART mode, wait for 10 seconds to change to automatic mode
//    * Delay 10 seconds to decrease nosise at startup.
//    * If we want automatic mode to go into really random mode, we need to generate some
//      randome seeds for it, we can operate this way:
//      Turn to UART mode
//      Turn on electricity
//      Turn to automatic mode in 10 seconds
//    * If not receive any operation in 10 seconds, turn to UART mode.
unsigned int bootwait (void)
{

        if ((BUTTON_PIN & (1 << MAIN_BTN))) // No input, PD2 pull-up high
	{
                LED_PORT |= (1 << LED_GREEN); // GReen for automatic mode
        	_delay_ms(5000);
                return 1;
	}
	
	// All the LED_PORT... code blinks the red and green status LEDs.  
	unsigned int x = 0;
	int ii = 0;
	while (1)
	{
                x++; // increment x by one.
		
		LED_PORT &= ~ (1 << LED_GREEN); // green off, red on
		LED_PORT |= (1 << LED_RED);

		for (ii=0; ii<1000; ii++ ) {
			_delay_ms(1);
			// Listen for button presses and return with the
			// apropriate number.
			if ((BUTTON_PIN & (1 << MAIN_BTN))) {
         			srand(ii); // use ii as random seed
		                LED_PORT &= ~ (1 << LED_RED);
                		LED_PORT |= (1 << LED_GREEN); // Green for automatic mode
			        return 3;
			}
		
		}
		LED_PORT &= ~ (1 << LED_RED); // red off, green on
		LED_PORT |= (1 << LED_GREEN);
		
		for (ii=0; ii<1000; ii++ ) {
			_delay_ms(1);
			// Same as above. I do it twise because there are two delays
			// in this loop, used for the red and green led blinking..
			if ((BUTTON_PIN & (1 << MAIN_BTN))) {
         			srand(x); // use ii  as random seed
		                LED_PORT &= ~ (1 << LED_RED);
                		LED_PORT |= (1 << LED_GREEN); // Green for automatic mode
			        return 3;
			}
		}
		if (x == 5) {
		        LED_PORT &= ~ (1 << LED_GREEN);
	                LED_PORT |= (1 << LED_RED); // RED for USART mode
			return 2;
		}
	}
}

// Take input from a computer and load it onto the cube buffer
void uart(void)
{
	int tempval;
	int x = 0;
	int y = 0;
    int escape = 0;
	
	while (1)
	{
		// Switch state on red LED for debugging
		// Should switch state every time the code
		// is waiting for a byte to be received.
		LED_PORT ^= (1 << LED_RED);

		// Wait until a byte has been received
		while ( !(UCSR0A & (1<<RXC0)) );

		// Load the received byte from uart into a buffer.
		tempval = UDR0;

		// Uncommet this to echo data back to the computer
		// for debugging purposes.
		//UDR0 = tempval;

		// Every time the cube receives a 0xff byte,
		// it goes into sync escape mode.
		// if a 0x00 byte is then received, the x and y counters
		// are reset to 0. This way the x and y counters are
		// always the same on the computer and in the cube.
		// To send an 0xff byte, you have to send it twice!

		// Go into sync escape mode
		if (tempval == 0xff)
		{
			// Wait for the next byte
			 while ( !(UCSR0A & (1<<RXC0)) );
			 // Get the next byte
			 tempval = UDR0;

			 // Sync signal is received.
			 // Reset x and y counters to 0.
			 if (tempval == 0x00)
			 {
				x = 0;
				y = 0;
                escape = 1;
			 }
			 // if no 0x00 byte is received, proceed with
			 // the byte we just received.
		}

        if (escape == 0)
        {
		// Load data into the current position in the buffer
		fb[x][y] = tempval;

    		// Check if we have reached the limits of the buffer array.
    		if (y == 7)
    		{
    			if (x == 7)
    			{
    				// All data is loaded. Reset both counters
    				y = 0;
    				x = 0;
                    // Copy the data onto the cube.
    				tmp2cube();
    			} else
    			{
    				// A layer is loaded, reset y and increment x.
    				x++;
    				y = 0;
    			}
    		} else
    		{
    			// We are in the middle of loading a layer. increment y.
    			y++;
    		}
	
	    } else
        {
            escape = 0;
        }
    }
}
