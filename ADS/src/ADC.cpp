#include "ADC.h"

ADCmodule::ADCmodule(int i2cAddress1, int i2cAddress2)
    : ads1(i2cAddress1), ads2(i2cAddress2)
{
    _i2cAddress1 = i2cAddress1;
    _i2cAddress2 = i2cAddress2;
}

void ADCmodule::config(byte channels, byte gain, byte mode, byte meas_rate, int errLED, int busyLED)

{
    _errLED = errLED;
    _busyLED = busyLED;

    digitalWriteFast(_busyLED, HIGH);

    

    //Check if ADS1115 modules are at the specified I²C-addresses

    //Begin transmission with first module and check for the ACK bit
    Wire.beginTransmission(_i2cAddress1);

    _connectionCheck = Wire.endTransmission();

    //If the transmission fails go into error mode
    if (_connectionCheck > 0)
    {
        //Print debug message and light up error LED
        Serial.println("ADC module error! \nNo ADS1115 with address 0x" + String(_i2cAddress1, HEX) + " found. \nCheck program and module");
        
        _t = millis();
        //Continously check if error was solved
        do
        {
            digitalWrite(_errLED, HIGH);
            Wire.beginTransmission(_i2cAddress1);
            _connectionCheck = Wire.endTransmission();
        } while (_connectionCheck > 0 && (millis() - _t) < 500);

        digitalWrite(_errLED, LOW);
    }

    //Check if second ADS1115 is at the specified I²C-address
    Wire.beginTransmission(_i2cAddress2);

    _connectionCheck = Wire.endTransmission();

    if (_connectionCheck > 0)
    {
        Wire.beginTransmission(_i2cAddress1);
        Serial.println("ADC module Error! \nNo ADS1115 with address 0x" + String(_i2cAddress2, HEX) + ". \nCheck program and module");

        _t = millis();
        do
        {
            digitalWrite(_errLED, HIGH);
            Wire.beginTransmission(_i2cAddress2);
            _connectionCheck = Wire.endTransmission();
        } while (_connectionCheck > 0 && (millis() - _t) < 500);

        digitalWrite(_errLED, LOW);
    }

    //Configure both ADS1115s
    ads1.config(gain, mode, meas_rate);
    ads2.config(gain, mode, meas_rate);

    //Set up the multiplexer and occupation arrays and count the occupied channels
    _channelCount = 0;
    for (_i = 0; _i < 8; _i++)
    {
        if (bitRead(channels, _i) == 1)
        {
            if (_i <= 3)
            {
                _MUX[_i] = _i;
                _occupied[_i] = true;
            }
            else if (_i > 3)
            {
                _MUX[_i] = _i % 4;
                _occupied[_i] = true;
            }
            _channelCount++;
        }
    }

    channelCount = _channelCount;

    digitalWriteFast(_busyLED, LOW);
}

void ADCmodule::readAll()
{
    digitalWriteFast(_busyLED, HIGH);

    _iVoltage = 0;

    for (_i = 0; _i < 4; _i++)
    {
        if (_occupied[_i])
        {
            ads1.startMeasurementToGND(_MUX[_i]);
        }

        if (_occupied[_i + 4])
        {
            ads2.startMeasurementToGND(_MUX[_i + 4]);
        }

        if (_occupied[_i] || _occupied[_i + 4])
        {
            delayMicroseconds(1300);

            if (_occupied[_i])
            {
                ads1.readConversion();
                Voltage[_iVoltage] = ads1.volts;
                _iVoltage++;
            }

            if (_occupied[_i + 4])
            {
                ads2.readConversion();
                Voltage[_iVoltage] = ads2.volts;
                _iVoltage++;
            }
        }
    }

    digitalWriteFast(_busyLED, LOW);
}

float ADCmodule::readSingle(int channel)
{
    digitalWriteFast(_busyLED, HIGH);

    if (channel <= 3 && _occupied[channel])
    {
        ads1.startMeasurementToGND(channel);

        delayMicroseconds(1300);

        ads1.readConversion();
        VoltageSingle = ads1.volts;
    }
    else if (channel > 3 && _occupied[channel])
    {
        ads2.startMeasurementToGND(channel);

        delayMicroseconds(1300);

        ads2.readConversion();
        VoltageSingle = ads2.volts;
    }
    else
    {
        digitalWriteFast(_errLED, HIGH);
        Serial.println("Channel " + String(channel) + " is not occupied!\nCheck channel configuration");
    }

    digitalWriteFast(_busyLED, LOW);

    return VoltageSingle;
}

void ADCmodule::defineLinearFormula(int channel, float offset, float slope)
{
    if (channel < 8 && _occupied[channel])
    {
        offsetCoeff[channel] = offset;
        slopeCoeff[channel] = slope;
        _definedFormula[channel] = true;
    }
    else
    {
        digitalWriteFast(_errLED, HIGH);
        Serial.println("Channel " + String(channel) + " is not occupied!\nCheck channel configuration");
    }
}

void ADCmodule::convertToUnitAll()
{
    digitalWriteFast(_busyLED, HIGH);

    for (_i = 0; _i < 8; _i++)
    {
        if (_definedFormula[_i] && _occupied[_i])
        {
            convertedValues[_i] = offsetCoeff[_i] + Voltage[_i] * slopeCoeff[_i];
        }
    }

    digitalWriteFast(_busyLED, LOW);
}

float ADCmodule::convertToUnitSingle(int channel)
{
    if (_definedFormula[channel])
    {
        convertedValueSingle = offsetCoeff[_i] + VoltageSingle * slopeCoeff[_i];
    }

    return convertedValueSingle;
}

int ADCmodule::faultDetection()
{
    //Check if ADS1115 modules are at the specified I²C-addresses

    //Begin transmission with first module and check for the ACK bit
    Wire.beginTransmission(_i2cAddress1);

    _connectionCheck = Wire.endTransmission();

    _t = millis();
    
    //If the transmission fails go into error mode
    if (_connectionCheck > 0)
    {
        //Print debug message and light up error LED
        Serial.println("ADC module error! \nNo ADS1115 with address 0x" + String(_i2cAddress1, HEX) + " found. \nCheck program and module");

        //Continously check if error was solved
        do
        {
            Wire.beginTransmission(_i2cAddress1);
            _connectionCheck = Wire.endTransmission();
        } while (_connectionCheck > 0 && millis() - _t < 500);
    }

    //Check if second ADS1115 is at the specified I²C-address
    Wire.beginTransmission(_i2cAddress2);

    _connectionCheck = Wire.endTransmission();

    if (_connectionCheck > 0)
    {
        Wire.beginTransmission(_i2cAddress1);
        Serial.println("ADC module Error! \nNo ADS1115 with address 0x" + String(_i2cAddress2, HEX) + ". \nCheck program and module");
        digitalWriteFast(_errLED, HIGH);
        fault = true;
    }
    return fault;
}