#include <MAIN.h>

MAINmodule::MAINmodule(int CSpinT, int I2CaddressP, int errLED, int busyLED)
    : sensorT(CSpinT), sensorP(Wire, I2CaddressP)
{
    _errLED = errLED;
    _busyLED = busyLED;

    _I2CaddressP = I2CaddressP;
}

void MAINmodule::config(int mode)
{
    pinMode(_errLED, OUTPUT);
    pinMode(_busyLED, OUTPUT);

    if (mode == 1)
    {
        sensorT.config(430, 1);
        if (sensorP.begin() != 1)
        {
            Serial.println("Der Umgebungsdrucksensor konnte nich initalisiert werden. Verkableung überprüfen");
            fault = true;
        }
    }
    else if (mode == 0)
    {
    }
}

void MAINmodule::busy()
{
    digitalWriteFast(_busyLED, HIGH);
}

void MAINmodule::notBusy()
{
    digitalWriteFast(_busyLED, LOW);
}

void MAINmodule::startTmeasurement()
{
    if (!Tmeasuring)
    {
        sensorT.startMeasurement();

        Tmeasuring = true;
        _tMeasurementStart = millis();
    }
}

float MAINmodule::readEnvT()
{
    if ((millis() - _tMeasurementStart) > 65)
    {
        envTemperature = sensorT.read();

        Tmeasuring = false;
    }

    return envTemperature;
}

float MAINmodule::readEnvP()
{
    sensorP.readSensor();
    envPressure = sensorP.getPressure_Pa() / 100;

    return envPressure;
}

bool MAINmodule::faultDetection()
{
    if(sensorP.begin() != 1)
    {
        Serial.println("Der Umgebungsdrucksensor konnte nich initalisiert werden. Verkableung überprüfen");
        fault = true;
    }
    else
    {
        fault = false;
    }
    
}

void MAINmodule::errorBlink(bool error)
{
    if(error && (millis()-_tErrLED >= 1000))
    {
        if(_errLEDon)
        {
        digitalWriteFast(_errLED,LOW);
        _tErrLED = millis();
        _errLEDon = false;
        }
        else if(!_errLEDon)
        {
            digitalWriteFast(_errLED,HIGH);
            _tErrLED = millis();
            _errLEDon = true;
        }
    }
    else if(!error)
    {
        digitalWriteFast(_errLED,LOW);
    }
}
