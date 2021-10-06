#include "RTD.h"

RTDmodule::RTDmodule(int i2c_addressRTD)
    : MAX31865module(0)
{
    _i2c_addressRTD = i2c_addressRTD;
}

void RTDmodule::config(byte sensorPosition, int Rref, int mode, int wiring = 1, int FaultDetection = 0, int filter = 1)
{
    _sensorPositions = sensorPosition;

    _sensorCount = 0

    MAX31865rtd.config(Rref, mode, wiring, FaultDetection, filter);

    for(_i = 0; _i < 4; _i++)
    {
        if(bitRead(_sensorPositions, _i))
        {
            _sensorCount++;
        }
    }

    Wire.beginTransmission(_i2c_addressTCO);
    Wire.write(0xFF);
    Wire.endTransmission();
}

void RTDmodule::startMeasurementSingle(int sensorNo)
{
    if(millis() - _tMeasurementStart[sensorNo] >= 65)
    {
    Wire.beginTransmission(_i2c_addressTCO);
    Wire.write(_sensorCS[sensorNo]);
    Wire.endTransmission();

    MAX31865rtd.startMeasurement();
    _tMeasurementStart[sensorNo] = millis();

    Wire.beginTransmission(_i2c_addressTCO);
    Wire.write(0xFF);
    Wire.endTransmission();
    }
    else
    {
        
    }
    
}

void RTDmodule::startMesasurementAll()
{
    for(_i = 0, _i < _sensorCount, _i++)
    {
        MAX31865rtd.startMesasurementSingle(_i);
    }
}

float RTDmodule::readTemperatureSingle(int sensorNo)
{
    Wire.beginTransmission(_i2c_addressTCO);
    Wire.write(_sensorCS[sensorNo]);
    Wire.endTransmission();

    TemperatureSingle = MAX31865rtd.read();

    Wire.beginTransmission(_i2c_addressTCO);
    Wire.write(0xFF);
    Wire.endTransmission();

    return TemperatureSingle;
}

void RTDmodule::readTemperatureAll()
{
    for(_i = 0, _i < _sensorCount, _i++)
    {
       TemperatureAll[_i] = readTemperatureSingle(_i);
    }
}
