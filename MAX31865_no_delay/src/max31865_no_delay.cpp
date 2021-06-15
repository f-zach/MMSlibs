#include "max31865_no_delay.h"

//Define the reference resistance of the reference resistior in Ohms

//Create pt100rtd object for use of the lookup tables
pt100rtd PT100 = pt100rtd();


// The constructor of the MAX31865 defines and configures the chip slect pin of the chip
//
// Input:
//  cs_pin:     The chip select pin for the MAX31865 
MAX31865::MAX31865(int cs_pin)
    : rtd(5000000,MSBFIRST,SPI_MODE1)
{
    pinMode(cs_pin, OUTPUT);
    _cs_pin = cs_pin;
    SPI.begin();
}

// In the config function different parameters of the MAX31865 can be set.
// The measurement mode can be chosen, the wiring configuration can be set, fault detection can be turned on and off (not implemented yet) and the 50/60Hz filter can be set
//
// Inputs:
//  Rref:       Value of the reference resistor (430 ohms for PT100)
//
//  mode:       0 → auto mode (continous measurement)
//              1 → normaly off (until one-shot measurement or changed to auto)
//
//  wiring:     0 → 2-wire or 4-wire configuration
//              1 → 3-wire configuration
//
//  faultDetection: 0 → fault detection off
//                  1 → fault detection on
//
// filter:       0 → 60Hz filter
//               1 → 50Hz filter
void MAX31865::config(int Rref, int mode, int wiring, int faultDetection, int filter)
{
//Save the bitshifted parameters as private vairables
    _Rref = Rref;

    _mode = mode << 6;

    _wiring = wiring << 4;

    _fualtDetection = faultDetection << 2;

    _filter = filter;

//Combine the paramters to a byte for a oneshot configuration and a byte to turn of the bias voltage to reduce power dissipation and self heating
    _startOneShot = _addr_write_config;
    _VBias_off = _addr_write_config;

    _configuration = 0 + _mode + _wiring + _fualtDetection + _filter;

    _startOneShot = _startOneShot << 8 | (_configuration + (B101 << 5));

    _VBias_off = _startOneShot << 8 | _configuration;
}

void MAX31865::startMeasurement()
{

    SPI.beginTransaction(rtd);
    //Drive the CS-pin low to select the MAX31865
    digitalWriteFast(_cs_pin, LOW);
    //Send the address byte to write (0x80) into the config register as well the configurtion byte
    //for a one-shot measurement
    SPI.transfer16(_startOneShot);
    //Drive CS-pin high to start the cinversion (!) and deselect the MAX31865
    digitalWriteFast(_cs_pin, HIGH);
    SPI.endTransaction();
}

// MAXreadT reads out the RTD from a MAX31865 and converts it to a Temperature value in °C.
// For the conversion to a temperature value the function uses the pt100rtd library. The RTD value
// is converted into the resistance according to an algorithm provided by the examples of the pt100rtd
// library. The correct temperature value is not calculated, but taken from lookup tables provided
// by the library.
// IMPORTANT: The conversion of a RTD value takes about 50ms in the configuration given by MAXoneShotStart.
//           Time your code accordingly to prevent reading old data.
float MAX31865::read()
{
    //Begin SPI tranaction with prdefined SPI settings
    SPI.beginTransaction(rtd);
    //Drive the CS-pin of the MAX31865 low to select the chip
    digitalWriteFast(_cs_pin, LOW);
    //Send the read address to read register of the raw RTD value
    SPI.transfer(_addr_read_converison);
    //Recieve the MSB and LSB of the raw RTD value
    _conversionValue = SPI.transfer16(0x0000);
    //Deselect the chip by driving the CS-pin low
    digitalWriteFast(_cs_pin, HIGH);

    SPI.endTransaction();

    //Convert the raw RTD value in a resistance value
    //For the conversion fomula reference the example given in the pt100rtd library
    conversionValue = _conversionValue;

    _dummy = (uint32_t)(_conversionValue) * 100 * (uint64_t)floor(_Rref);
    _dummy >>= 16;
    //pt100ohmsx is the calculated resistance value of the PT100
    _Resistance = (uint16_t)(_dummy & 0xFFFF);
    Resistance = _Resistance;

    //Convert the resistance value into a temperature with the help of the lookup tables
    //provided by the pt100rtd library. For more information reference the example given
    //in the library
    Temperature = _PT100.celsius(_Resistance);
    return Temperature;
}
