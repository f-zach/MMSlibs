#include <Arduino.h>
#include <FreqMeasure.h>

long t;
float frequency;
byte sendBuffer[4], *p = sendBuffer;
const int LEDPin = 35;

bool measurementActive = true;

void setup()
{
  pinMode(LEDPin,OUTPUT);

  Serial.begin(9600);
  Serial1.begin(115200);
  FreqMeasure.begin();
}

double sum = 0;
int count = 0;

void loop()
{
  //Do this loop until the read symbol is recieved
  while (measurementActive)
  {
    if (FreqMeasure.available())
    {
      // average several reading together
      sum = sum + FreqMeasure.read();
      count = count + 1;
    }
    
    //Stop measuremoent on recieved 'r'
    if (Serial1.available() > 0)
    {
      if (Serial1.read() == 'r')
      {
        measurementActive = false;
      }
    }
  }

  digitalWriteFast(LEDPin,HIGH);

  //Calculate frequency
  if (count != 0)
  {
    frequency = FreqMeasure.countToFrequency(sum / count);
  }
  //if measurement count is 0 treat
  else
  {
    frequency = 0;
  }
  
  //Copy float value of frequency to 4 byte sendBuffer
  memcpy(sendBuffer, &frequency, 4);

  Serial1.write(sendBuffer, 4);
  Serial.println(frequency);


  sum = 0;
  count = 0;

  digitalWriteFast(LEDPin,LOW);

  measurementActive = true;

}
