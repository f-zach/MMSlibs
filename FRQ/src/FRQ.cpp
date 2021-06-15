#include "FRQ.h"

FRQmodule::FRQmodule(long baudRate)
{
    Serial1.begin(baudRate);
}

float FRQmodule::getFrequency()
{
    Serial1.write('r');

    i = 0;
    while (Serial1.available() > 0)
    {
        _freqBuffer[i] = Serial1.read();
        i++;
    }

    memcpy(&frequency, _freqBuffer, 4);

    return frequency;
}