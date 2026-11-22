/**
 * VÍ DỤ 2: QUẢN LÝ THẺ NHỚ BẰNG LỆNH KIỂU LINUX
 */
#include <Arduino.h>
#include <PTITCube.h>

PTIT_Storage storage;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\nKhoi dong he thong luu tru...");
    storage.init();
    
    if (storage.isReady()) {
        Serial.println("SD Card da san sang (go 'help' de xem danh sach lenh)");
    } else {
        Serial.println("Storage: SD not ready");
    }
}

void loop() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if (cmd.length() > 0) {
            Serial.println("> " + cmd);
            String response = storage.handleCommand(cmd);
            if (response.length() > 0) {
                Serial.println(response);
            }
        }
    }
}
