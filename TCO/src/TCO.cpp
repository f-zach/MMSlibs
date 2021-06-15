#include "TCO.h"

TCOmodule::TCOmodule(int i2c_addressTCO)
{
    _i2c_addressTCO = i2c_addressTCO;
}

void TCOmodule::config(byte sensorPositions, bool checkFault, int errLED, int busyLED)
{
    _errLED = errLED;
    _busyLED = busyLED;

    digitalWriteFast(_busyLED, HIGH);

    _sensorPositions = sensorPositions;

    //Count amount of defined Sensors
    _sensorCount = 0;
    for (_i = 0; _i < 8; _i++)
    {
        if (bitRead(sensorPositions, _i))
        {
            _sensorCS[_sensorCount] = B11111111 ^ (B00000001 << _i);
            _sensorPosArr[_sensorCount] = _i + 1;
            _sensorCount++;
        }
    }

    sensorCount = _sensorCount;

    //Set I2C expander outputs  (CS pins) to high
    Wire.beginTransmission(_i2c_addressTCO);
    Wire.write(0xFF);
    Wire.endTransmission();

    //Check if all modules are present/set up correctly
    if (checkFault)
    {
        checkFaultAll();
    }

    digitalWriteFast(_busyLED, LOW);
}

int TCOmodule::checkFaultAll()
{
    //Read the wo least significant bytes from all MAX31855
    readInternalDataAll();

    for (_i = 0; _i < _sensorCount; _i++)
    {
        //If the data is 0 there is no connection to the MAX31855 module
        if (!rawInternalData[_i])
        {
            _fault = true;
            Serial.println("Sensor " + String(_i + 1) + " auf Position " + String(_sensorPosArr[_i]) + " antwortet nicht \nKonfiguration des Moduls überprüfen");
        }
        //If any of the 3 least significant bits is nonzero there is a thermocouple fault
        else if (rawInternalData[_i] & 0x07)
        {
            _fault = true;
            Serial.println("Das Thermocouple an Sensor " + String(_i + 1) + " auf Position " + String(_sensorPosArr[_i]) + " ist nicht korrekt angeschlossen:");
            switch (rawInternalData[_i] & 0x07)
            {
            case 1: //Open circuit on Thermocouple
                Serial.println("Offener Schaltkreis \nVerkabelung des Thermocouples überprüfen");
                break;
            case 3: //Thermocouple short-circuited to GND
                Serial.println("Thermocouple ist zum GND kurzgeschlossen \nVerkabelung des Thermocouples überprüfen");
                break;
            case 4: //Thermocouple short-circuited to 3V3
                Serial.println("Thermocouple ist zur Versorgungspannun kurzgeschlossen \nVerkabelung des Thermocouples überprüfen");
                break;
            }
        }
        else
        {
            _fault = false;
        }
    }

    return _fault;
}

int TCOmodule::readDataAll()
{
    //Loop through all sensors
    for (_i = 0; _i < _sensorCount; _i++)
    {
        //Set sensor CS to LOW on the I²C port expander
        Wire.beginTransmission(_i2c_addressTCO);
        Wire.write(_sensorCS[_i]);
        Wire.endTransmission();

        //Read the two most signifcant bytes from the sensor and save them to the data array
        rawData[_i] = SPI.transfer16(0x00);
    }

    //Set all CS pins to HIGH
    Wire.beginTransmission(_i2c_addressTCO);
    Wire.write(B11111111);
    Wire.endTransmission();

    return 0;
}

int TCOmodule::readDataSingle(int sensorNo)
{
    //Set sensor CS to LOW on the I²C port expander
    Wire.beginTransmission(_i2c_addressTCO);
    Wire.write(_sensorCS[sensorNo - 1]);
    Wire.endTransmission();

    //Read the two most signifcant bytes from the sensor
    rawData[_i] = SPI.transfer16(0x00);

    //Set all CS pins to HIGH
    Wire.beginTransmission(_i2c_addressTCO);
    Wire.write(B11111111);
    Wire.endTransmission();

    return 0;
}

float TCOmodule::convertAll(bool unit)
{
    //Loop through all values in the data array
    for (_i = 0; _i < _sensorCount; _i++)
    {
        if (rawData[_i] & 0x8000) //Conversion for negative temperature values
        {
            _dataVal = 0xC000 | ((rawData[_i] >> 2) & 0x3FFF);
        }
        else //Conversion for positive temperature values
        {
            _dataVal = rawData[_i] >> 2;
        }

        //Conversion to Celsius or Kelvin
        if (!unit)
        {
            TemperatureC[_i] = (float)_dataVal * 0.25;
        }
        else if (unit)
        {
            TemperatureK[_i] = (float)_dataVal * 0.25 - 273.15;
        }
    }

    return 0;
}

float TCOmodule::convertSingle(int sensorNo, bool unit)
{

    if (rawData[sensorNo - 1] & 0x8000) //Conversion for negative temperature values
    {
        _dataVal = 0xC000 | ((rawData[sensorNo - 1] >> 2) & 0x3FFF);
    }
    else //Conversion for positive temperature values
    {
        _dataVal = rawData[sensorNo - 1] >> 2;
    }

    //Conversion to Celsius or Kelvin
    if (!unit)
    {
        TemperatureC[sensorNo - 1] = (float)_dataVal * 0.25;
    }
    else if (unit)
    {
        TemperatureK[sensorNo - 1] = (float)_dataVal * 0.25 - 273.15;
    }

    return 0;
}

float TCOmodule::readTempAll(bool unit)
{
    //Read all sensor data and convert it to temperature value
    readDataAll();
    convertAll(unit);

    return 0;
}

float TCOmodule::readTempSingle(int sensorNo, bool unit)
{
    //Read data from a single sensor and convert it to a temperature value
    readDataSingle(sensorNo);
    convertSingle(unit);

    return 0;
}

float TCOmodule::readInternalDataAll()
{
    //Loop through all sensors defined in the configuration
    for (_i = 0; _i < _sensorCount; _i++)
    {
        //Set sensor CS to LOW on the I²C port expander
        Wire.beginTransmission(_i2c_addressTCO);
        Wire.write(_sensorCS[_i]);
        Wire.endTransmission();

        //Read the two least signifcant bytes from the sensor
        _dataBuf = SPI.transfer16(0x00);
        _dataBuf = _dataBuf << 16 | SPI.transfer16(0x00);
        _dataBuf &= 0x00FF;

        //and save the value to the data array
        rawInternalData[_i] = _dataBuf;
    }

    //Set all CS pins to HIGH
    Wire.beginTransmission(_i2c_addressTCO);
    Wire.write(B11111111);
    Wire.endTransmission();

    return 0;
}
