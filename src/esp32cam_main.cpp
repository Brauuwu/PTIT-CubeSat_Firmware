#include <Arduino.h>
#include <Camera/Camera.h>

PTIT_Camera camera;
unsigned long captureNumber = 0;

String nextCapturePath() {
    return "/photos/capture_" + String(++captureNumber) + ".jpg";
}

void handleCommand(String command) {
    command.trim();
    command.toLowerCase();

    if (command == "ping") {
        Serial.println("CAMERA:OK");
        return;
    }

    if (command == "status") {
        Serial.println(camera.isReady() ? "CAMERA:READY" : "CAMERA:ERROR");
        return;
    }

    if (command == "capture" || command == "take_photo") {
        String path = nextCapturePath();
        bool captured = camera.captureToFile(path.c_str());
        Serial.println(captured ? "CAPTURE:OK " + path : "CAPTURE:ERROR");
        return;
    }

    if (command.length() > 0) {
        Serial.println("CAMERA:UNKNOWN_COMMAND");
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("ESP32-CAM command firmware");

    if (!camera.init(255, 255)) {
        Serial.println("CAMERA:INIT_ERROR");
    } else {
        Serial.println("CAMERA:READY");
    }
}

void loop() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        handleCommand(command);
    }
    delay(2);
}