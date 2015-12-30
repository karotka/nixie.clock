#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "ds3232rtc.h"

#define RX_BUFFER_SIZE 10
#define DIGITS 6

unsigned char serialRXbuffer[RX_BUFFER_SIZE];
unsigned char serialRXstatus;
uint8_t serialRXBufferCount = 0;
uint8_t timer1counter = 0;
uint8_t i = 0;
int dimmer = 0;

char digitsc[DIGITS];
static TWI_t *twi = &TWIC;
static DSRTC_t *rtc;

void pinsInit();
void setUp16MhzExternalOsc();
void SevenSegmentChar(char ch, uint8_t dp);
void timer0Init();
void timer1Init();
void PrintChr(char c[]);
void clearPorts();
void setPort(const uint8_t i);


ISR(TCD0_OVF_vect) { // 488Hz
    char str[7];

    sprintf (str, "%02d%02d%02d", rtc->hour, rtc->minute, rtc->second);
    PrintChr(str);
    SevenSegmentChar(digitsc[i], 0);

    clearPorts();
    setPort(i);
    if (i == DIGITS - 1) {
        i = 0;
    } else {
        i++;
    }
}

ISR(TCC1_OVF_vect) { // 30Hz

    // every 0.5s update clock second
    if (timer1counter == 15) {
        timer1counter = 0;

        RTC_readSecond(twi, rtc);

        if (rtc->second == 0) {
            RTC_readMinute(twi, rtc);
            RTC_readHour(twi, rtc);
            RTC_readTemperature(twi, rtc);
        }
    }
    timer1counter++;
}

int main(void) {

    setUp16MhzExternalOsc();
    TWI_MasterInit(twi);

    pinsInit();
    PMIC.CTRL = PMIC_LOLVLEN_bm;

    timer0Init();
    timer1Init();
    USART_init();
    sei();

    char str[20];

    for(;;) {
        sprintf (str, "%d:%d:%d %.1fC \n", rtc->hour, rtc->minute,
            rtc->second, rtc->temperature);
        USART_sendString(str);
        _delay_ms(1000);
    }
}

ISR(USARTC0_RXC_vect) {
    char str[50];

    unsigned char c = USARTC0.DATA;

    serialRXbuffer[serialRXBufferCount++] = c;
    if (serialRXBufferCount == 1) {
        serialRXstatus = c;
    }

    if (c == '\n') {
        if (serialRXstatus == 'T') {
            RTC_writeSecond(twi, 0);
            RTC_writeHour(twi, serialRXbuffer[1]);
            RTC_writeMinute(twi, serialRXbuffer[2]);
            sprintf(str, "Set time: %d:%d \n", serialRXbuffer[1], serialRXbuffer[2]);
        }

        if (serialRXstatus == 'L') {
            TCD0_CCABUF = serialRXbuffer[1];
            TCD0_CCBBUF = serialRXbuffer[2];
            TCD0_CCCBUF = serialRXbuffer[3];
            dimmer = (uint8_t)serialRXbuffer[4];

            sprintf(str, "Set light: %d, %d, %d, %d.\n",
                    serialRXbuffer[1], serialRXbuffer[2],
                    serialRXbuffer[3], serialRXbuffer[4]);
        }

        USART_sendString(str);
        serialRXBufferCount = 0;
        serialRXstatus = 0;
    }
}

void timer0Init() {
    TCD0_PER = 128;               // Set the period of the waveform

    TCD0_CCABUF = 0;              // set the duty cycle A
    TCD0_CCBBUF = 0;              // set the duty cycle B
    TCD0_CCCBUF = 0;              // set the duty cycle C
    TCD0_CCDBUF = 0;              // set the duty cycle D

    TCD0_CTRLA |= 0x06;           // clock selection clk/64
    TCD0_CTRLB |= 0x03;           // Single slope mode
    TCD0_CTRLB |= 0xf0;           // channel selection CCAEN, CCBEN, CCCEN, CCDEN

    /* timer0 OVF enabled */
    TCD0.INTCTRLA |= TC_OVFINTLVL_LO_gc;
}

void timer1Init() {
    TCC1.PER = 255;               // Set the period of the waveform

    TCC1.CTRLA |= 0x07;           // clock slection clk/1024
    TCC1_CTRLB |= 0x03;           // Single slope mode

    /* timer1 OVF enabled */
    TCC1.INTCTRLA |= TC_OVFINTLVL_LO_gc;
}

void pinsInit() {
    // PORTD as output
    PORTD.DIR = 0xff;
    PORTD.DIRSET |= PIN4_bm;
    PORTD.DIRSET |= PIN5_bm;
    PORTD.DIRSET |= PIN6_bm;
    PORTD.DIRSET |= PIN7_bm;

    // PORTE
    PORTE.DIR = 0xff;
    PORTE.DIRSET |= PIN0_bm;
    PORTE.DIRSET |= PIN1_bm;

    // PORTC as output
    //PORTC.DIR = 0xff;
    PORTC.DIRSET = PIN3_bm; /* PD3 (TXD0) as output. */
    PORTC.DIRCLR = PIN2_bm; /* PD2 (RXD0) as input. */
    PORTC.DIRCLR = PIN0_bm;
    PORTC.DIRCLR = PIN1_bm;

    // PORTA as output
    PORTA.DIR = 0xff;
}


/**
 * This function writes a char given by n to the display
 * the decimal point is displayed if dp=1
 */
void SevenSegmentChar(char ch, uint8_t dp) {
    switch (ch) {
    case ' ':
        PORTA.OUT=0b11111111;
        break;
    case '0':
        PORTA.OUT=0b10000000;
        break;
    case 'O':
        PORTA.OUT=0b10000000;
        break;
    case '1':
        PORTA.OUT=0b11110011;
        break;
    case '2':
        PORTA.OUT=0b01001001;
        break;
    case '3':
        PORTA.OUT=0b01100000;
        break;
    case '4':
        PORTA.OUT=0b00110010;
        break;
    case '5':
        PORTA.OUT=0b00100100;
        break;
    case '6':
        PORTA.OUT=0b00000100;
        break;
    case '7':
        PORTA.OUT=0b11110000;
        break;
    case '8':
        PORTA.OUT=0b00000000;
        break;
    case '9':
        PORTA.OUT=0b00110000;
        break;
    case 'A':
        PORTA.OUT=0b11101110;
        break;
    case 'C':
        PORTA.OUT=0b10011100;
        break;
    case 'F':
        PORTA.OUT=0b10001110;
        break;
    case 'L':
        PORTA.OUT=0b00011100;
        break;
    case 'n':
        PORTA.OUT=0b00101010;
        break;
    case 'f':
        PORTA.OUT=0b10001110;
        break;
    case 'i':
        PORTA.OUT=0b00100000;
        break;
    case 'd':
        PORTA.OUT=0b01111010;
        break;
    case 'e':
        PORTA.OUT=0b11011110;
        break;
    case 'u':
        PORTA.OUT=0b00111000;
        break;
    case 'U':
        PORTA.OUT=0b01111100;
        break;
    case 'Y':
        PORTA.OUT=0b01100110;
        break;
    case 'G':
        PORTA.OUT=0b10111110;
        break;
    case 'N':
        PORTA.OUT=0b11101100;
        break;
    case 'P':
        PORTA.OUT=0b00011001;
        break;
    case 'E':
        PORTA.OUT=0b01111100;
        break;
    case 'S':
        PORTA.OUT=0b10110110;
        break;
    case '-':
        PORTA.OUT=0b00000010;
        break;
    }

    if(dp) {
        // If decimal point should be displayed
        // Make 0th bit Low
        PORTA.OUT |= 0b00000001;
    }
}

void PrintChr(char c[]) {
    unsigned int i = 0;
    unsigned int j = DIGITS - 1;

    for(i = 0; i < DIGITS; i++) {
        digitsc[i] = c[j];
        j--;
    }
}

void clearPorts() {
    PORTD.OUT &= 0xf;   // 0b0000....
    PORTE.OUT &= 0xfc;  // 0b......00
}

void setPort(const uint8_t i) {
    switch (i) {
    case 0:
        PORTD.OUTSET |= PIN4_bm;
        break;
    case 1:
        PORTD.OUTSET |= PIN5_bm;
        break;
    case 2:
        PORTD.OUTSET |= PIN6_bm;
        break;
    case 3:
        PORTD.OUTSET |= PIN7_bm;
        break;
    case 4:
        PORTE.OUTSET |= PIN0_bm;
        break;
    case 5:
        PORTE.OUTSET |= PIN1_bm;
        break;
    }
}

void setUp16MhzExternalOsc() {
    /* Enable 16MHz external crystal oscilator */
    OSC_XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
    OSC_CTRL |= OSC_XOSCEN_bm;

    /* Wait for oscillator ready */
    while(!(OSC_STATUS & OSC_XOSCRDY_bm));

    /* Selects clock system as external clock
       through change protection mechanism */
    CCP = CCP_IOREG_gc;
    /* Select external 16Mhz oscilator */
    CLK_CTRL = CLK_SCLKSEL_XOSC_gc;

    /* Disable 2Mhz oscillator */
    OSC_CTRL &= ~OSC_RC2MEN_bm;
}

void setUp32MhzInternalOsc() {
    /* Enable 32Mhz crystal oscilator */
    OSC_CTRL |= OSC_RC32MEN_bm;

    /* Wait for oscillator ready */
    while(!(OSC_STATUS & OSC_RC32MRDY_bm));

    /* Selects clock system as external clock
       through change protection mechanism */
    CCP = CCP_IOREG_gc;
    /* Select internal 32Mhz oscilator */
    CLK_CTRL = CLK_SCLKSEL_RC32M_gc;

    /* Disable 2Mhz oscillator */
    OSC_CTRL &= ~OSC_RC2MEN_bm;
}
