#include "twi.h"

// DS3232 I2C Address
#define RTCADDR 0xd0

//DS3232 Register Addresses
#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x01
#define RTC_HOURS 0x02
#define RTC_DAY 0x03
#define RTC_DATE 0x04
#define RTC_MONTH 0x05
#define RTC_YEAR 0x06
#define ALM1_SECONDS 0x07
#define ALM1_MINUTES 0x08
#define ALM1_HOURS 0x09
#define ALM1_DAYDATE 0x0A
#define ALM2_MINUTES 0x0B
#define ALM2_HOURS 0x0C
#define ALM2_DAYDATE 0x0D
#define RTC_CONTROL 0x0E
#define RTC_STAT 0x0F
#define RTC_AGING 0x10
#define TEMP_MSB 0x11
#define TEMP_LSB 0x12
#define SRAM_START_ADDR 0x14    //first SRAM address
#define SRAM_SIZE 236           //number of bytes of SRAM

//Alarm mask bits
#define A1M1 7
#define A1M2 7
#define A1M3 7
#define A1M4 7
#define A2M2 7
#define A2M3 7
#define A2M4 7

//Control register bits
#define EOSC 7
#define BBSQW 6
#define CONV 5
#define RS2 4
#define RS1 3
#define INTCN 2
#define A2IE 1
#define A1IE 0

//Status register bits
#define OSF 7
#define BB32KHZ 6
#define CRATE1 5
#define CRATE0 4
#define EN32KHZ 3
#define BSY 2
#define A2F 1
#define A1F 0


//Square-wave output frequency (TS2, RS1 bits)
enum SQWAVE_FREQS_t {SQWAVE_1_HZ, SQWAVE_1024_HZ, SQWAVE_4096_HZ, SQWAVE_8192_HZ, SQWAVE_NONE};

//Alarm masks
enum ALARM_TYPES_t {
    ALM1_EVERY_SECOND = 0x0F,
    ALM1_MATCH_SECONDS = 0x0E,
    ALM1_MATCH_MINUTES = 0x0C,     //match minutes *and* seconds
    ALM1_MATCH_HOURS = 0x08,       //match hours *and* minutes, seconds
    ALM1_MATCH_DATE = 0x00,        //match date *and* hours, minutes, seconds
    ALM1_MATCH_DAY = 0x10,         //match day *and* hours, minutes, seconds
    ALM2_EVERY_MINUTE = 0x8E,
    ALM2_MATCH_MINUTES = 0x8C,     //match minutes
    ALM2_MATCH_HOURS = 0x88,       //match hours *and* minutes
    ALM2_MATCH_DATE = 0x80,        //match date *and* hours, minutes
    ALM2_MATCH_DAY = 0x90,         //match day *and* hours, minutes
};

#define ALARM_1 1                  //constants for calling functions
#define ALARM_2 2

//Other
#define DS1307_CH 7                //for DS1307 compatibility, Clock Halt bit in Seconds register
#define HR1224 6                   //Hours register 12 or 24 hour mode (24 hour mode==0)
#define CENTURY 7                  //Century bit in Month register
#define DYDT 6                     //Day/Date flag bit in alarm Day/Date registers

struct RTC_t {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    float temperature;
};

//void setAlarm(ALARM_TYPES_t alarmType, byte seconds, byte minutes, byte hours, byte daydate);
//void setAlarm(ALARM_TYPES_t alarmType, byte minutes, byte hours, byte daydate);
//void alarmInterrupt(byte alarmNumber, bool alarmEnabled);
//bool alarm(byte alarmNumber);
//void squareWave(SQWAVE_FREQS_t freq);
//bool oscStopped(bool clearOSF = true);  //defaults to clear the OSF bit if argument not supplied
//int temperature(void);

uint8_t RTC_readSecond(TWI_t *twi);
uint8_t RTC_readMinute(TWI_t *twi);
uint8_t RTC_readHour(TWI_t *twi);
float RTC_readTemperature(TWI_t *twi);

void RTC_writeSecond(TWI_t *twi, uint8_t data);
void RTC_writeMinute(TWI_t *twi, uint8_t data);
void RTC_writeHour(TWI_t *twi, uint8_t data);

uint8_t dec2bcd(uint8_t n);
uint8_t bcd2dec(uint8_t n);
