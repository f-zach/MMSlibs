#include "FRQ.h"

FRQmodule::FRQmodule(long baudRate)
{
    Serial1.begin(baudRate);
}

float FRQmodule::getFrequency(bool twoFreq)
{
    Serial1.write('r');

    i = 0;
    while (Serial1.available() > 0)
    {
        _freqBuffer[i] = Serial1.read();
        i++;
    }

    memcpy(&frequency1, _freqBuffer, 4);

    if (twoFreq)
    {
        Serial.write('n')

            while (Serial1.available() > 0)
        {
            _freqBuffer[i] = Serial1.read();
            i++;
        }

        memcpy(&frequency2, _freqBuffer, 4);
    }
    else
    {
        Serial.write('0')
    }

    return frequency1, frequency2;
}