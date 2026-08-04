#include "GPS.h"

volatile bool ppsTriggered = false;
void IRAM_ATTR ppsHandler() { ppsTriggered = true; }

PTIT_GPS::PTIT_GPS() : gpsSerial(2) {
}

void PTIT_GPS::init() {
    Serial.println("[GPS] Initializing GPS NEO (UART2)...");
    gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX2=16, TX2=17
    
    pinMode(4, INPUT_PULLDOWN); // PPS=4
    attachInterrupt(digitalPinToInterrupt(4), ppsHandler, RISING);
}

void PTIT_GPS::update() {
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }
}

float PTIT_GPS::getLat() {
    if (gps.location.isValid()) return gps.location.lat();
    return 0.0;
}

float PTIT_GPS::getLng() {
    if (gps.location.isValid()) return gps.location.lng();
    return 0.0;
}

float PTIT_GPS::getAltitude() {
    if (gps.altitude.isValid()) return gps.altitude.meters();
    return 0.0;
}

int PTIT_GPS::getSatellites() {
    if (gps.satellites.isValid()) return gps.satellites.value();
    return 0;
}

float PTIT_GPS::getSpeed() {
    if (gps.speed.isValid()) return gps.speed.kmph();
    return 0.0;
}

float PTIT_GPS::getCourse() {
    if (gps.course.isValid()) return gps.course.deg();
    return 0.0;
}

float PTIT_GPS::getHDOP() {
    if (gps.hdop.isValid()) return gps.hdop.hdop();
    return 0.0;
}

String PTIT_GPS::getTimeString() {
    if (gps.date.isValid() && gps.time.isValid()) {
        char sz[25];
        snprintf(sz, sizeof(sz), "%04d-%02d-%02dT%02d:%02d:%02dZ",
                 gps.date.year(), gps.date.month(), gps.date.day(),
                 gps.time.hour(), gps.time.minute(), gps.time.second());
        return String(sz);
    }
    return "N/A";
}
