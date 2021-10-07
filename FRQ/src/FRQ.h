#include <Arduino.h>

class FRQmodule
{
private:
    byte _freqBuffer1[4], _freqBuffer2[4];
    int i;
public:
    FRQmodule(long baudRate);
    float getFrequency(bool twoFreq);
    float frequency1, frequency2;

};
