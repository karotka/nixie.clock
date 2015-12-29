#include <stdio.h>
#include <avr/io.h>

void TWI_MasterInit(TWI_t *twi);

void TWI_writeByte(TWI_t *twi, uint8_t position, uint8_t data);
uint8_t TWI_readByte(TWI_t *twi, uint8_t position);

inline void TWI_StopTransmission(TWI_t* const twi);
void TWI_waitIdle();
