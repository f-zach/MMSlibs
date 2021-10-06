#ifndef RTD_MOD
#define RTD_MOD

#include <Arduino.h>
#include <max31865_no_delay.h>
#include <SPI.h>

#define RREF 430.0

class RTDmodule
{
private:
   int _i2c_addressRTD;
   byte _sensorPositions;
   int _sensorCount;
   int _i;
   byte _sensorCS[4] = {B00000001, B00000010, B00000100, B00001000};
   long _tMeasurementStart[4];

public:
    RTDmodule(int i2c_addressRTD);
    void config(byte sensorPosition, int Rref, int mode, int wiring = 1, int FaultDetection = 0, int filter = 1);
    void startMeasurementSingle(int sensorNo);
    void startMesasurementAll();
    float readTemperatureSingle(int sensorNo);
    void readTemperatureAll();
    int TemperatureSingle;
    int TemperatureAll[4];
    MAX31865 MAX31865rtd;
};

