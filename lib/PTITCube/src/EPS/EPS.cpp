#include "EPS.h"
#include <Arduino.h>

#define BATTERY_PIN 34 // Pin ADC dùng để đo điện áp pin
#define V_REF 3.3
#define R1 100000.0 // Điện trở R1 (100k)
#define R2 100000.0 // Điện trở R2 (100k)

void PTIT_EPS::init() {
    Serial.println("[EPS] Initializing Power System...");
    pinMode(BATTERY_PIN, INPUT);
}

void PTIT_EPS::update() {
    // Monitor power states
}

float PTIT_EPS::getBatteryVoltage() {
    int adc = analogRead(BATTERY_PIN);
    float v_pin = (adc / 4095.0) * V_REF;
    float v_bat = v_pin * ((R1 + R2) / R2); // Tính điện áp thực tế qua cầu phân áp
    return v_bat;
}
