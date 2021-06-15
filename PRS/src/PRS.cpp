#include "PRS.h"

PRSmodule::PRSmodule(int i2cAddress)
{
    _i2cAddress = i2cAddress;
}

void PRSmodule::config(byte sensorPositions, float PressureRange[8], byte diff, int UnitSensor[8], int UnitReq[], bool checkFault, int errLED, int busyLED)
{
    _errLED = errLED;
    _busyLED = busyLED;



    _sensorCount = 0;

    for(_i = 0; _i < 8; _i++)
    {
        if(bitRead(sensorPositions,_i))
        {
            _ChannelNo[_sensorCount] = _i;
            _Sensor[_i][0] = PressureRange[_i];
            _Sensor[_i][1] = bitRead(diff, _i);
            _Sensor[_i][2] = UnitSensor[_i];

            _sensorCount++;
        }
    }

    SensorCount = _sensorCount;
}

void PRSmodule::switchToChannel(int ChannelNo)
{
    _channelByte = B00000001 << ChannelNo;

    Wire.beginTransmission(_i2cAddress);
    Wire.write(_channelByte);
    Wire.endTransmission();
}

void PRSmodule::closeAllChannels()
{
   Wire.beginTransmission(_i2cAddress);
    Wire.write(0);
    Wire.endTransmission();
}

int PRSmodule::readDataSingle(int ChannelNo)
{
    switchToChannel(ChannelNo);

    Wire.requestFrom(0x28,2);
    rawDataSingle = (Wire.read() << 8 | Wire.read()) & 0x3FFF;

    return rawDataSingle;
}

void PRSmodule::readDataAll()
{
    
    for(_i = 0; _i < _sensorCount; _i++)
    {
       _rawData[_i] = readDataSingle(_ChannelNo[_i]);
       rawData[_i] = _rawData[_i];
    }
}

float PRSmodule::convertSingle(int DataVal, float PressureRange, bool diff, int UnitSensor, int UnitReq)
{  
    if(!diff)
    {
        PressureSingle = (((DataVal - 1638) * PressureRange) / 13107) * _unitConversion[UnitSensor][UnitReq];
    }

    else if(diff)
    {
        
        PressureSingle = ((((DataVal - 1638) * (PressureRange - (-PressureRange))) / 13107) - PressureRange) * _unitConversion[UnitSensor][UnitReq];
    }

    return PressureSingle;
}

void PRSmodule::convertAll()
{
    for(_i = 0; _i < _sensorCount; _i++)
    {
        
        Pressure[_i] = convertSingle(_rawData[_i], _Sensor[_ChannelNo[_i]][0],_Sensor[_ChannelNo[_i]][1], _Sensor[_ChannelNo[_i]][2], _Sensor[_ChannelNo[_i]][3]);
    }
}

float PRSmodule::readPressSingle(int ChannelNo)
{
    
    rawDataSingle = readDataSingle(ChannelNo);
    closeAllChannels();

    PressureSingle = convertSingle(rawDataSingle, _Sensor[ChannelNo][0], _Sensor[ChannelNo][1], _Sensor[ChannelNo][2], _Sensor[ChannelNo][3]);

    return PressureSingle;
}

void PRSmodule::readPressAll()
{
    readDataAll();
    closeAllChannels();
       
    convertAll();

}
