#include "ADS1115adc.h"


// Constructor for the ADS1115adc class. It is used to define the ADS1115adc object with its I²C-address
//
// Input:
//   i2c_ads: I²C-adress of the ADS1115

ADS1115adc::ADS1115adc(int i2c_ads)
{
    _i2c_ads = i2c_ads;
}


// Configure the adc object.
// Options for the configuration are the gain (voltage range) of the adc, the measurement mode and the measurement rate in sample per second (SPS)
// 
// Input:
//   gain:  0 → ±6.144V
//          1 → ±4.096V
//          2 → ±2.048V
//          3 → ±1.024V
//          4 → ±0.512V
//          5 → ±0.256V
//  
//  mode:   0 → continous mode
//          1 → single shot mode (recommended)
//
//  meas_rate:  0 → 8 SPS
//              1 → 16 SPS
//              2 → 32 SPS
//              3 → 64 SPS
//              4 → 128 SPS
//              5 → 250 SPS
//              6 → 475 SPS
//              7 → 860 SPS    

void ADS1115adc::config(byte gain, byte mode, byte meas_rate)
{
//Save all the parameters to the the private variables of the class
//The different paramters are bitshifted accordingly so they can be combined into two bytes for the configuration of the ADS1115 
    _gain = gain << 1;
    _volt_range = _ranges[gain];

    _mode = mode;

    _mode_bool = (bool)mode;

    _meas_rate = meas_rate << 5;

 //Combine the parameters of the most significant config byte according to the chosen measurement mode  
    if(!_mode_bool)
    {
        _config_MSB = _config_unmod_continous_MSB + _gain + _mode;
    }
    else if(_mode_bool)
    {
        _config_MSB = _config_unmod_oneShot_MSB + _gain + _mode;
    }

    config_MSB = _config_MSB;

//Add the measurement rate to the least significant config byte
    _config_LSB = _config_unmod_LSB + _meas_rate;

    config_LSB = _config_LSB;

//Write the address of the config register in an array with the config MSB and LSB
    _config[0] = _addr_config;
    _config[1] = _config_MSB;
    _config[2] = _config_LSB;

//Transmit the config data to the ADS1115
    Wire.beginTransmission(_i2c_ads);
    Wire.write(_config,3);
    Wire.endTransmission();
}

// Start a one-shot voltage measutrement on one of the MUX-channels compared to ground (0V)
//
// Inputs:
//  MUX:    0 → Channel 0
//          1 → Channel 1
//          2 → Channel 2
//          3 → Channel 3
void ADS1115adc::startMeasurementToGND(int MUX)
{
//Reset the MSB of the config data
    _config[1] = _config_MSB;

//Switch to determine the data to be added to the config MSB
    switch(MUX)
    {
        case 0:
        _config[1] += B100 << 4;
        break;

        case 1:
        _config[1] += B101 << 4;
        break;

        case 2:
        _config[1] += B110 << 4;
        break;

        case 3:
        _config[1] += B111 << 4;
        break;
    }

// Transmit the data to start the one-shot measurement
    Wire.beginTransmission(_i2c_ads);
    Wire.write(_config,3);
    Wire.endTransmission();    

}

// Start a one-shot voltage measutrement on one of the MUX-channels compared to ground (0V)
//
// Inputs:
//  MUX:    0 → Channel 0 & 1
//          1 → Channel 0 & 3
//          2 → Channel 1 & 3
//          3 → Channel 2 & 3

void ADS1115adc::startDifferentialMeasurement(int MUX)
{
//Reset the MSB of the config data
    _config[1] = _config_MSB;

//Switch to determine the data to be added to the config MSB
    switch(MUX)
    {
        case 0:
        _config[1] += B000 << 4;
        break;

        case 1:
        _config[1] += B001 << 4;
        break;

        case 2:
        _config[1] += B010 << 4;
        break;

        case 3:
        _config[1] += B011 << 4;
        break;
    }

// Transmit the data to start the one-shot measurement
    Wire.beginTransmission(_i2c_ads);
    Wire.write(_config,3);
    Wire.endTransmission();    

}

// Read the last conversion of the ADS1115 (one-shot or continous)
// The values of the raw converison value or the Voltage values can be accessed via the class of the ADS1115
//      → for example someADS1115adcObjectName.conversionValue() or someADS1115adcObjectName.volts()
// The voltage value is returned in [V]

void ADS1115adc::readConversion()
{
//Write the address of the conversion register
    Wire.beginTransmission(_i2c_ads);
    Wire.write(_addr_conversion);
    Wire.endTransmission();

//Read the two bytes of the conversion value and write them to the output variable of the ADS1115adc object
    Wire.requestFrom(_i2c_ads,2);
    _conversion = Wire.read() << 8 | Wire.read();
    conversionValue = _conversion;

//Convert the converison value to a voltage value
    volts = ((float)_conversion * _volt_range)/32768;
}

