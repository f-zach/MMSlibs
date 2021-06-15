#include <Arduino.h>

class FRQmodule
{
private:
    byte _freqBuffer[4];
    int i;
public:
    FRQmodule(long baudRate);
    float getFrequency();
    float frequency;

};
