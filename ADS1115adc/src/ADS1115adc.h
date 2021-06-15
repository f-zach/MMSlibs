#ifndef ADS1115ADC_H
#define ADS1115ADC_H

#include <Arduino.h>
#include <i2c_t3.h>

class ADS1115adc
{
    public:
        ADS1115adc(int i2c_ads);
        void config(byte gain = 0, byte mode = 1, byte meas_rate = 7);
        void startMeasurementToGND(int MUX);
        void startDifferentialMeasurement(int MUX);
        void readConversion();
        float volts;
        int16_t conversionValue;
        byte config_MSB;
        byte config_LSB;
    private:
        int _i2c_ads;
        byte _gain;
        float _ranges[6] = {6.144, 4.096, 2.048, 1.024, 0.512, 0.256};
        float _volt_range;
        byte _meas_rate;
        byte _mode;
        byte _config[3];
        bool _mode_bool;
        byte _addr_config = 0x01;
        byte _addr_conversion = 0x00;
        byte _config_unmod_oneShot_MSB = B10000000;
        byte _config_unmod_continous_MSB = B00000000;
        byte _config_unmod_LSB = B00000011;
        byte _config_MSB;
        byte _config_LSB;
        int16_t _conversion;
};

#endif