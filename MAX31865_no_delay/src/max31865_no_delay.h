#ifndef MAX31865_H
#define MAX31865_H

#include <Arduino.h>
#include <SPI.h>
#include <pt100rtd.h>

class MAX31865
{
    public:
        MAX31865(int cs_pin);
        void config(int Rref, int mode, int wiring = 1, int faultDetection = 0, int filter = 1);
        void startMeasurement();
        float read();
        float conversionValue;
        float Temperature;
        float Resistance;
    private:
        byte _conversionValueArr[2];
        uint16_t _conversionValue;
        uint32_t _dummy;
        uint16_t _Resistance;
        


        int _cs_pin;  
        float _Rref;
        int _mode;
        int _wiring;
        int _fualtDetection;
        int _filter;
        byte _configuration;
        uint16_t _startOneShot;
        uint16_t _VBias_off;
        byte _addr_write_config = 0x80;    
        byte _addr_read_config = 0x00;      
        byte _addr_read_converison = 0x01;

        pt100rtd _PT100 = pt100rtd();
        SPISettings rtd;

};

#endif //MAX31865_H
