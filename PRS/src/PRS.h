#include <Arduino.h>
#include <i2c_t3.h>

class PRSmodule
{
private:
    int _i2cAddress;
    int _errLED;
    int _busyLED;
    int _i;
    int _sensorCount;
    int _ChannelNo[8];
    float _Sensor[8][4];
    byte _channelByte;
    int _rawData[8];
    const float _unitConversion[3][3] = {{1,0.001,68.947572932},{1000,1,0.0689475729},{14.503773773,0.0145037738,1}};


    


public:
    PRSmodule(int i2cAddress);
    void config(byte sensorPositions, float PressureRange[8], byte diff, int UnitSensor[8], int UnitReq[8], bool checkFault = false, int errLED = 5, int busyLED = 6);
    void switchToChannel(int ChannelNo);
    void closeAllChannels();
    int readDataSingle(int ChannelNo);
    void readDataAll();
    float convertSingle(int DataVal, float PressureRange, bool diff, int UnitSensor, int UnitReq);
    void convertAll();
    float readPressSingle(int ChannelNo);  
    void readPressAll();
    int SensorCount;
    int rawData[8];
    int rawDataSingle;
    float Pressure[8];
    float PressureSingle;
    

};


