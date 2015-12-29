#include <avr/io.h>
#include "twi.h"
#include <util/delay.h>

/**
 * PORT PIN SIG
 * PE0  28  SDA
 * PE1  29  SCL
 *
 **/
#define BAUDRATE 500000L
#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)
#define TWI_BAUDSETTING TWI_BAUD(F_CPU, BAUDRATE)

#define WRITE   0
#define READ    1

void TWI_MasterInit(TWI_t *twi) {
    twi->CTRL = 0x00;
    twi->MASTER.BAUD = TWI_BAUDSETTING;
    twi->MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
    twi->MASTER.CTRLB = TWI_MASTER_TIMEOUT_DISABLED_gc;
    twi->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
    twi->MASTER.STATUS |= TWI_MASTER_BUSSTATE_IDLE_gc;
}

uint8_t TWI_readByte(TWI_t *twi, const uint8_t address, const uint8_t position) {
    //if((twi->MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) != TWI_MASTER_BUSSTATE_IDLE_gc);

    uint8_t i = 0;

    twi->MASTER.ADDR = address;
    while(!(twi->MASTER.STATUS & TWI_MASTER_WIF_bm));

    twi->MASTER.DATA = position;

    twi->MASTER.ADDR = address + READ;
    while (!(twi->MASTER.STATUS & TWI_MASTER_RIF_bm));
	i = twi->MASTER.DATA;

    TWI_StopTransmission(twi);

    return i;
}

void TWI_writeByte(TWI_t *twi, const uint8_t address, const uint8_t position, const uint8_t data) {
    //while((twi->MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) != TWI_MASTER_BUSSTATE_IDLE_gc);

    twi->MASTER.ADDR = address | WRITE;
    while(!(twi->MASTER.STATUS & TWI_MASTER_WIF_bm));
    if (twi->MASTER.STATUS & TWI_MASTER_RXACK_bm) {
        return;
    }

    twi->MASTER.DATA = position;
    while(!(twi->MASTER.STATUS & TWI_MASTER_WIF_bm));
    if (twi->MASTER.STATUS & TWI_MASTER_RXACK_bm) {
        return;
    }

    twi->MASTER.DATA = data;
    while(!(twi->MASTER.STATUS & TWI_MASTER_WIF_bm));
    if (twi->MASTER.STATUS & TWI_MASTER_RXACK_bm) {
        return;
    }

    TWI_StopTransmission(twi);
}

inline void TWI_StopTransmission(TWI_t* const twi) {
    twi->MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
}
