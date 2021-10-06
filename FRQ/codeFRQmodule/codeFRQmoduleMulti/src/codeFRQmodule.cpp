#include <Arduino.h>
#include <FreqMeasure.h>
#include <FreqMeasureMulti.h>

FreqMeasureMulti freq1;
FreqMeasureMulti freq2;

float frequency1, frequency2;
byte sendBuffer1[4], *p1 = sendBuffer1, sendBuffer2[4], *p2 = sendBuffer1;
const int LEDPin1 = 19, LEDPin2 = 20;
bool freq2active = true;

bool measurementActive = true;

void setup()
{
  pinMode(LEDPin1, OUTPUT);
  pinMode(LEDPin2, OUTPUT);

  Serial.begin(9600);
  Serial1.begin(115200);

  freq1.begin(9);

  if (freq2active)
  {
    freq2.begin();
  }
}

double sum1 = 0, sum2 = 0;
int count1 = 0, count2 = 0;

void loop()
{
  //Do this loop until the read symbol is recieved
  while (measurementActive)
  {
    if (freq1.available())
    {
      // average several reading together
      sum1 = sum1 + freq1.read();
      count1 = count1 + 1;
    }

    if (freq2active && freq2.available)
    {
      // average several reading together
      sum2 = sum2 + freq2.read();
      count2 = count2 + 1;
    }

    //Stop measurement on recieved 'f'
    if (Serial1.available() > 0)
    {
      if (Serial1.read() == 'f')
      {
        measurementActive = false;
      }
    }
  }

  digitalWriteFast(LEDPin1, HIGH);

  //Calculate frequency
  if (count1 != 0)
  {
    frequency1 = freq1.countToFrequency(sum1 / count1);
  }
  //if measurement count is 0 treat
  else
  {
    frequency1 = 0;
  }

  memcpy(sendBuffer1, &frequency1, 4);

  Serial1.write(sendBuffer1, 4);

  digitalWriteFast(LEDPin1, LOW);
  while (!Serial.available())
  {
  }

  if (Seial.read() == 'n')
  {
    digitalWriteFast(LEDPin2, HIGH);
    if (freq2active && count2 != 0)
    {
      frequency2 = freq2.countToFrequency(sum2 / count2)
    }
    else if (freq2active &&count2 = 0)
    {
      frequency2 = 0;
    }

    memcpy(sendBuffer2, &frequency2, 4);

    Serial.write(sendBuffer2, 4);
  }

  Serial.println(frequency1);
  Serial.println(frequency2);

  double sum1 = 0, sum2 = 0;
  int count1 = 0, count2 = 0;

  digitalWriteFast(LEDPin, LOW);

  measurementActive = true;
}
