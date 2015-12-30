#include "ds3232rtc.h"

void RTC_readSecond(TWI_t *twi, DSRTC_t *rtc) {
    rtc->second = bcd2dec(TWI_readByte(twi, RTCADDR, RTC_SECONDS));
}

void RTC_readMinute(TWI_t *twi, DSRTC_t *rtc) {
    rtc->minute = bcd2dec(TWI_readByte(twi, RTCADDR, RTC_MINUTES));
}

void RTC_readHour(TWI_t *twi, DSRTC_t *rtc) {
    rtc->hour = bcd2dec(TWI_readByte(twi, RTCADDR, RTC_HOURS));
}

void RTC_readTemperature(TWI_t *twi, DSRTC_t *rtc) {

    int8_t temp3232a = TWI_readByte(twi, RTCADDR, TEMP_MSB);
    uint8_t temp3232b = TWI_readByte(twi, RTCADDR, TEMP_LSB);

    float temp3232 = (float)temp3232a;
    //for positive temp
    if((temp3232b == 0x40) && (temp3232a >= 0)) temp3232 += 0.25;
    if (temp3232b == 0x80) temp3232 += 0.5;
    if((temp3232b == 0xc0) && (temp3232a >= 0)) temp3232 += 0.75;
    //for negative temp
    if((temp3232b == 0x40) && (temp3232a < 0)) temp3232 += 0.75;
    if((temp3232b == 0xc0) && (temp3232a < 0)) temp3232 += 0.25;
    rtc->temperature = bcd2dec(temp3232);
    /*
    uint8_t msb;
    uint8_t lsb;

    msb = TWI_readByte(twi, TEMP_MSB);
    lsb = TWI_readByte(twi, TEMP_LSB);

    return ((int)msb * 100) + ((int)lsb / 4);*/
}

void RTC_writeSecond(TWI_t *twi, const uint8_t data) {
    TWI_writeByte(twi, RTCADDR, RTC_SECONDS, dec2bcd(data));
}

void RTC_writeMinute(TWI_t *twi, const uint8_t data) {
    TWI_writeByte(twi, RTCADDR, RTC_MINUTES, dec2bcd(data));
}

void RTC_writeHour(TWI_t *twi, const uint8_t data) {
    TWI_writeByte(twi, RTCADDR, RTC_HOURS, dec2bcd(data));
}


//void DS3232RTC::setAlarm(ALARM_TYPES_t alarmType, byte seconds, byte minutes, byte hours, byte daydate)
//{
//    uint8_t addr;
//
//    seconds = dec2bcd(seconds);
//    minutes = dec2bcd(minutes);
//    hours = dec2bcd(hours);
//    daydate = dec2bcd(daydate);
//    if (alarmType & 0x01) seconds |= _BV(A1M1);
//    if (alarmType & 0x02) minutes |= _BV(A1M2);
//    if (alarmType & 0x04) hours |= _BV(A1M3);
//    if (alarmType & 0x10) hours |= _BV(DYDT);
//    if (alarmType & 0x08) daydate |= _BV(A1M4);
//
//    if ( !(alarmType & 0x80) ) {    //alarm 1
//        addr = ALM1_SECONDS;
//        writeRTC(addr++, seconds);
//    }
//    else {
//        addr = ALM2_MINUTES;
//    }
//    writeRTC(addr++, minutes);
//    writeRTC(addr++, hours);
//    writeRTC(addr++, daydate);
//}
//
///*----------------------------------------------------------------------*
// * Set an alarm time. Sets the alarm registers only.  To cause the      *
// * INT pin to be asserted on alarm match, use alarmInterrupt().         *
// * This method can set either Alarm 1 or Alarm 2, depending on the      *
// * value of alarmType (use a value from the ALARM_TYPES_t enumeration). *
// * However, when using this method to set Alarm 1, the seconds value    *
// * is set to zero. (Alarm 2 has no seconds register.)                   *
// *----------------------------------------------------------------------*/
//void DS3232RTC::setAlarm(ALARM_TYPES_t alarmType, byte minutes, byte hours, byte daydate)
//{
//    setAlarm(alarmType, 0, minutes, hours, daydate);
//}
//
///*----------------------------------------------------------------------*
// * Enable or disable an alarm "interrupt" which asserts the INT pin     *
// * on the RTC.                                                          *
// *----------------------------------------------------------------------*/
//void DS3232RTC::alarmInterrupt(byte alarmNumber, bool interruptEnabled)
//{
//    uint8_t controlReg, mask;
//
//    controlReg = readRTC(RTC_CONTROL);
//    mask = _BV(A1IE) << (alarmNumber - 1);
//    if (interruptEnabled)
//        controlReg |= mask;
//    else
//        controlReg &= ~mask;
//    writeRTC(RTC_CONTROL, controlReg);
//}
//
///*----------------------------------------------------------------------*
// * Returns true or false depending on whether the given alarm has been  *
// * triggered, and resets the alarm flag bit.                            *
// *----------------------------------------------------------------------*/
//bool DS3232RTC::alarm(byte alarmNumber)
//{
//    uint8_t statusReg, mask;
//
//    statusReg = readRTC(RTC_STATUS);
//    mask = _BV(A1F) << (alarmNumber - 1);
//    if (statusReg & mask) {
//        statusReg &= ~mask;
//        writeRTC(RTC_STATUS, statusReg);
//        return true;
//    }
//    else {
//        return false;
//    }
//}
//
///*----------------------------------------------------------------------*
// * Enable or disable the square wave output.                            *
// * Use a value from the SQWAVE_FREQS_t enumeration for the parameter.   *
// *----------------------------------------------------------------------*/
//void DS3232RTC::squareWave(SQWAVE_FREQS_t freq)
//{
//    uint8_t controlReg;
//
//    controlReg = readRTC(RTC_CONTROL);
//    if (freq >= SQWAVE_NONE) {
//        controlReg |= _BV(INTCN);
//    }
//    else {
//        controlReg = (controlReg & 0xE3) | (freq << RS1);
//    }
//    writeRTC(RTC_CONTROL, controlReg);
//}
//
///*----------------------------------------------------------------------*
// * Returns the value of the oscillator stop flag (OSF) bit in the       *
// * control/status register which indicates that the oscillator is or    *
// * was stopped, and that the timekeeping data may be invalid.           *
// * Optionally clears the OSF bit depending on the argument passed.      *
// *----------------------------------------------------------------------*/
//bool DS3232RTC::oscStopped(bool clearOSF)
//{
//    uint8_t s = readRTC(RTC_STATUS);    //read the status register
//    bool ret = s & _BV(OSF);            //isolate the osc stop flag to return to caller
//    if (ret && clearOSF) {              //clear OSF if it's set and the caller wants to clear it
//        writeRTC( RTC_STATUS, s & ~_BV(OSF) );
//    }
//    return ret;
//}
//
///*----------------------------------------------------------------------*
// * Returns the temperature in Celsius times four.                       *
// *----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*
 * Decimal-to-BCD conversion                                            *
 */
uint8_t dec2bcd(uint8_t n) {
    return n + 6 * (n / 10);
}

///*----------------------------------------------------------------------*
// * BCD-to-Decimal conversion                                            *
// *----------------------------------------------------------------------*/
uint8_t __attribute__ ((noinline)) bcd2dec(uint8_t n) {
    return n - 6 * (n >> 4);
}
