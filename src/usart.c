#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"

void USART_init() {
    /**
     * Baud rate selection
     * BSEL = (32000000 / (1 * 16 * 9600) -1
     * FBAUD = 16000000.0 / (pow(2, 0) * 16 * (103 + 1))
     *   Baudrate table 16Mhz
     *   --------------------
     *   Rate       UBRR    UBRR    % of error
     *   300        3332	0x0D04	0.0
     *   600        1666	0x0682	0.0
     *   1200       832     0x0340	0.0
     *   2400       416     0x01A0	0.0
     *   4800       207     0x00CF	0.2
     *   9600       103     0x0067	0.2
     *   14400      68      0x0044	0.6
     *   19200      51      0x0033	0.2
     *   28800      34      0x0022	0.8
     *   38400      25      0x0019	0.2
     *   57600      16      0x0010	2.1
     *   76800      12      0x000C	0.2
     *   115200     8       0x0008	3.7
     *   230400     3       0x0003	7.8
     *   250000     3       0x0003	0.0
     */
    USARTC0_BAUDCTRLB = 0; // Just to be sure that BSCALE is 0
    USARTC0_BAUDCTRLA = 0x67; // 103 = 9600b

    // Enable low lewel interrupt
    USARTC0.CTRLA = USART_RXCINTLVL_LO_gc;

    // 8 data bits, no parity and 1 stop bit
    USARTC0_CTRLC = USART_CHSIZE_8BIT_gc;

    // Enable receive and transmit
    USARTC0_CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

void USART_sendChar(char c) {
    while( !(USARTC0_STATUS & USART_DREIF_bm) ); // Wait for empty transmit buffer
    USARTC0_DATA = c;
}

void USART_sendString(char *str) {
    while(*str) {
        USART_sendChar(*str++);
    }
}

void USART_sendInt(const int val) {
    char buffer[8];
    USART_sendString(itoa(val, buffer, 10));
}

char USART_receiveByte() {
    while( !(USARTC0_STATUS & USART_RXCIF_bm) ); //Interesting DRIF didn't work.
    return USARTC0_DATA;
}

unsigned int USART_recvInt(int arr[2]) {
    return 256*arr[1] + arr[0];
}
