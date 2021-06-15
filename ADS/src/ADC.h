#include <Arduino.h>
#include <i2c_t3.h>
#include "ADS1115adc.h"


class ADCmodule
{
private:
    int _i2cAddress1;
    int _i2cAddress2;
    int _conversionValue;
    int _channelCount;
    int _i, _iVoltage;
    int _MUX[8];
    bool _occupied[8];
    bool _definedFormula[8];
    int _errLED;
    int _busyLED;
    int _connectionCheck;
    long _t;
    
public:
    ADCmodule(int i2cAddress1, int i2cAddress2);
    void config(byte channels, byte gain = 0, byte mode = 1, byte meas_rate = 7,int errLED = 5, int busyLED = 6);
    int faultDetection();
    void readAll();
    float readSingle(int channel);
    void defineLinearFormula(int channel, float offset, float slope);
    void convertToUnitAll();
    float convertToUnitSingle(int channel);
    int channelCount;
    float Voltage[8];
    float VoltageSingle;
    float convertedValues[8];
    float convertedValueSingle;
    float offsetCoeff[8];
    float slopeCoeff[8];
    bool fault;
    ADS1115adc ads1;
    ADS1115adc ads2;



};
