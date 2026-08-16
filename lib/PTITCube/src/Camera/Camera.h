#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>

class PTIT_Camera {
public:
    PTIT_Camera();

    bool init(uint8_t gpsSelectPin = 12, uint8_t camSelectPin = 14);
    void selectGPS();
    void selectCamera();
    bool capture();
    bool captureToFile(const char* path = "/photos/capture.jpg");
    bool isReady() const;

private:
    uint8_t gpsSelectPin;
    uint8_t camSelectPin;
    bool cameraReady;
    bool sdReady;

    bool initializeCamera();
    bool initializeSd();
};

#endif
