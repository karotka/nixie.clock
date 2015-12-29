#include <stdio.h>
#include <avr/io.h>

#ifndef TWI_H
#define TWI_H

void TWI_MasterInit(TWI_t *twi);

void TWI_writeByte(TWI_t *twi, const uint8_t address, const uint8_t position, const uint8_t data);
uint8_t TWI_readByte(TWI_t *twi, const uint8_t address, const uint8_t position);

inline void TWI_StopTransmission(TWI_t* const twi);
void TWI_waitIdle();

#endif
