#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPS++.h>

class PTIT_GPS {
public:
    PTIT_GPS();
    
    void init();
    void update();
    float getLat();
    float getLng();
    float getAltitude();
    int getSatellites();
    float getSpeed();
    float getCourse();
    float getHDOP();
    String getTimeString();

private:
    TinyGPSPlus gps;
    HardwareSerial gpsSerial;
};

#endif
