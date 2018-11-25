/* Functions to initialize, send, receive over USART

   initUSART requires BAUD to be defined in order to calculate
     the bit-rate multiplier.
 */

// Define USART stuff
#define FOSC 16000000UL
//#define BAUD 76800UL
#define BAUD 115200UL
//#define BAUD 38400UL


/* Takes the defined BAUD and F_CPU,
   calculates the bit-clock multiplier,
   and configures the hardware USART                   */
void initUSART(void);