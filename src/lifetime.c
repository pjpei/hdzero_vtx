#include "lifetime.h"
#include "print.h"


extern uint16_t seconds ;

uint32_t sysLifeTime = 0;
uint32_t sysLifeTime_last = 0;
uint8_t LifeTimeOP = 0;
void Get_EEP_LifeTime(void)
{
    uint8_t u8;

    u8 = I2C_Read(ADDR_EEPROM, EEP_ADDR_LIFETIME_0, 0, 0);
    sysLifeTime = (uint32_t)u8;
    u8 = I2C_Read(ADDR_EEPROM, EEP_ADDR_LIFETIME_1, 0, 0);
    sysLifeTime += (uint32_t)u8 << 8;
    u8 = I2C_Read(ADDR_EEPROM, EEP_ADDR_LIFETIME_2, 0, 0);
    sysLifeTime += (uint32_t)u8 << 16;
    u8 = I2C_Read(ADDR_EEPROM, EEP_ADDR_LIFETIME_3, 0, 0);
    sysLifeTime += (uint32_t)u8 << 24;

    if(sysLifeTime == 0xffffffff)
    {
        WAIT(10); I2C_Write(ADDR_EEPROM, EEP_ADDR_LIFETIME_0, 0x00, 0, 0);
        WAIT(10); I2C_Write(ADDR_EEPROM, EEP_ADDR_LIFETIME_1, 0x00, 0, 0);
        WAIT(10); I2C_Write(ADDR_EEPROM, EEP_ADDR_LIFETIME_2, 0x00, 0, 0);
        WAIT(10); I2C_Write(ADDR_EEPROM, EEP_ADDR_LIFETIME_3, 0x00, 0, 0);
        sysLifeTime = 0;
    }

    sysLifeTime_last = sysLifeTime;

}
void Update_EEP_LifeTime(void)
{
    uint8_t u8;
    uint32_t diff;
    static uint16_t lstSeconds = 0;

#ifdef _DEBUG_MODE
    return;
#endif
#ifdef VIDEO_PAT
    return;
#endif
#ifdef _RF_CALIB
    return;
#endif
    
    if(seconds - lstSeconds >= 10)
    {
        sysLifeTime ++;
        lstSeconds = seconds;
        if(sysLifeTime >= 3599999)
            sysLifeTime = 3599999;
    }
    else
    {
        return;
    }

    diff = sysLifeTime_last ^ sysLifeTime;

    #ifdef _DEBUG_LIFETIME
    Printf("\r\nsysLifeTime:%ld",sysLifeTime);
    #endif
    
    if((diff >> 0) & 0xff)
    {
        u8 = (sysLifeTime >> 0)& 0xff;
        WAIT(10); I2C_Write(ADDR_EEPROM, EEP_ADDR_LIFETIME_0, u8, 0, 0);
    }

    if((diff >> 8) & 0xff)
    {
        u8 = (sysLifeTime >> 8) & 0xff;
        WAIT(10); I2C_Write(ADDR_EEPROM, EEP_ADDR_LIFETIME_1, u8, 0, 0);
    }

    if((diff >> 16) & 0xff)
    {
        u8 = (sysLifeTime >> 16) & 0xff;
        WAIT(10); I2C_Write(ADDR_EEPROM, EEP_ADDR_LIFETIME_2, u8, 0, 0);
    }

    if((diff >> 24) & 0xff)
    {
        u8 = (sysLifeTime >> 24) & 0xff;
        WAIT(10); I2C_Write(ADDR_EEPROM, EEP_ADDR_LIFETIME_3, u8, 0, 0);
    }

    sysLifeTime_last = sysLifeTime;

}

void ParseLifeTime(unsigned char *hourString, unsigned char *minuteString)
{
    uint32_t minute;
    uint32_t hour;

    hour = sysLifeTime/360;
    hourString[0] = '0' + (hour%10000)/1000;
    hourString[1] = '0' + (hour%1000)/100;
    hourString[2] = '0' + (hour%100)/10;
    hourString[3] = '0' + (hour%10);

    if(hourString[0] == '0')
    {
        hourString[0] = ' ';
        if(hourString[1] == '0')
        {
            hourString[1] = ' ';
            if(hourString[2] == '0')
            {
                hourString[2] = ' ';
            }
        }
    }
    
    minute= (sysLifeTime%360)/6;
    minuteString[0] = '0' + (minute%60)/10;
    minuteString[1] = '0' + (minute%10);
}