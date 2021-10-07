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

  digitalWriteFast(LEDPin1,HIGH);
  delay(250);

  Serial.begin(9600);

  digitalWriteFast(LEDPin1,LOW);  
  digitalWriteFast(LEDPin2,HIGH);
  delay(250);

  Serial1.begin(115200);

  digitalWriteFast(LEDPin1,HIGH);
  digitalWriteFast(LEDPin2,LOW);
  delay(250);

  freq1.begin(9);

  if (freq2active)
  {
    freq2.begin(10);
  }
  
  for(int j = 0; j<8; j++)
  {
    digitalToggleFast(LEDPin1);
    digitalToggleFast(LEDPin2);
    delay(250);
  }

  digitalWriteFast(LEDPin1,LOW);
  digitalWriteFast(LEDPin2,LOW);
  
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
      //Sum the measured signal cycle times and count measurements for frequency 1
      sum1 = sum1 + freq1.read();
      count1 = count1 + 1;
    }

    if (freq2active && freq2.available())
    {
      //Sum the measured signal cycle times and count measurements for frequency 2
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

  //Calculate frequency 1
  if (count1 != 0)
  {
    frequency1 = freq1.countToFrequency(sum1 / count1);
  }
  //If measurement count is 0 set frequency 1 to 0
  else
  {
    frequency1 = 0;
  }

  //Copy frequency 1 to send buffer
  memcpy(sendBuffer1, &frequency1, 4);

  //Send the buffer via Serial1
  Serial1.write(sendBuffer1, 4);

  //Print frequency 1 over the USB serial port
  Serial.println(frequency1);

  digitalWriteFast(LEDPin1, LOW);

  //Wait until another command symbol arrives
  while (!Serial1.available())
  {
  }

  //If a 'n' is recieved send frequency 2
  if (Serial1.read() == 'n')
  {
    digitalWriteFast(LEDPin2, HIGH);

    //Check if measurement of frequency 2 is activated
    if (freq2active && count2 != 0)
    {
      //Calculate frequency 2
      frequency2 = freq2.countToFrequency(sum2 / count2);
    }
    else if (freq2active && count2 == 0)
    {
      //If count 2 is zero set frequency 2 to zero
      frequency2 = 0;
    }

    //Copy frequency 2 to send buffer
    memcpy(sendBuffer2, &frequency2, 4);

    //Send the buffer via Serial1
    Serial1.write(sendBuffer2, 4);

    //
    Serial.println(frequency2);

    digitalWriteFast(LEDPin2,LOW);
  }
  
  Serial.println(frequency2);

  sum1 = 0, sum2 = 0;
  count1 = 0, count2 = 0;

  Serial1.flush();

  measurementActive = true;
}
