#include <Arduino.h>
#include <SPI.h>
#include <i2c_t3.h>


class TCOmodule
{
private:
    int _i2c_addressTCO;
    int _sensorPositions;
    int _errLED;
    int _busyLED;
    int _sensorCount;
    int _sensorCS[8];
    int _sensorPosArr[8];
    int _dataBuf;
    int _dataVal;
    int _i;
    bool _fault;
    long _t;
    
public:
    TCOmodule(int i2c_addressTCO);
    void config(byte sensorPositions, bool checkFault, int errLED = 5, int busyLED = 6);
    int checkFaultAll();
    int checkFaultSingle();
    int readDataAll();
    int readDataSingle(int sensorNo);
    float convertAll(bool unit = 0);
    float convertSingle(int sensorNo, bool unit = 0);
    float readTempAll(bool unit = 0);
    float readTempSingle(int sensorNo, bool unit = 0);
    float readInternalDataAll();
    float readInternalDataSingle(int sensorNo);    
    int rawData[8];
    int rawDataSingle;
    int rawInternalData[8];
    float TemperatureC[8];
    float TemperatureK[8];
    int sensorCount;
    
};

