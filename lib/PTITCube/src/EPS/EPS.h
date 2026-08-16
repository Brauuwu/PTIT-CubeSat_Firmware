#ifndef EPS_H
#define EPS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MAX1704X.h>

class PTIT_EPS {
public:
    PTIT_EPS();

    void init();
    void update();
    float getBatteryVoltage();
    float getBatteryPercent();

private:
    Adafruit_MAX17048 max17048;
    bool max17048Ready;
    float lastVoltage;
    float lastPercent;
};

#endif
