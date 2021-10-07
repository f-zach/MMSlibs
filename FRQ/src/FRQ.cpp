#include "FRQ.h"

FRQmodule::FRQmodule(long baudRate)
{
    Serial1.begin(baudRate);
}

float FRQmodule::getFrequency(bool twoFreq)
{
    Serial1.write('f');
    delay(1);
    i = 0;
    while (Serial1.available() > 0)
    {
        _freqBuffer1[i] = Serial1.read();
        i++;
    }

    memcpy(&frequency1, _freqBuffer1, 4);

    if (twoFreq)
    {
        Serial1.write('n');

        delay(1);
        i = 0;

        while (Serial1.available() > 0)
        {
            _freqBuffer2[i] = Serial1.read();
            i++;
        }

        memcpy(&frequency2, _freqBuffer2, 4);
    }
    else
    {
        Serial1.write('0');
    }

    Serial1.flush();

}