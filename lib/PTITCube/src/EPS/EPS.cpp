#include "EPS.h"

#define BATTERY_PIN 34 // Pin ADC dùng để đo điện áp pin
#define V_REF 3.3
#define R1 100000.0 // Điện trở R1 (100k)
#define R2 100000.0 // Điện trở R2 (100k)

PTIT_EPS::PTIT_EPS() : max17048Ready(false), lastVoltage(0.0f), lastPercent(0.0f) {
}

void PTIT_EPS::init() {
    Serial.println("[EPS] Initializing Power System...");
    Wire.begin();
    pinMode(BATTERY_PIN, INPUT);

    if (max17048.begin()) {
        max17048Ready = true;
        Serial.println("[EPS] MAX17048 detected via I2C (SDA=21, SCL=22)");
        lastVoltage = max17048.cellVoltage();
        lastPercent = max17048.cellPercent();
    } else {
        max17048Ready = false;
        Serial.println("[EPS] MAX17048 not found, using ADC fallback.");
    }
}

void PTIT_EPS::update() {
    if (max17048Ready) {
        lastVoltage = max17048.cellVoltage();
        lastPercent = max17048.cellPercent();
    }
}

float PTIT_EPS::getBatteryVoltage() {
    if (max17048Ready) {
        return max17048.cellVoltage();
    }

    int adc = analogRead(BATTERY_PIN);
    float v_pin = (adc / 4095.0) * V_REF;
    float v_bat = v_pin * ((R1 + R2) / R2);
    lastVoltage = v_bat;
    return v_bat;
}

float PTIT_EPS::getBatteryPercent() {
    if (max17048Ready) {
        return max17048.cellPercent();
    }

    float voltage = getBatteryVoltage();
    float percent = 0.0f;

    if (voltage >= 4.2f) percent = 100.0f;
    else if (voltage <= 3.0f) percent = 0.0f;
    else percent = ((voltage - 3.0f) / (4.2f - 3.0f)) * 100.0f;

    lastPercent = percent;
    return percent;
}
