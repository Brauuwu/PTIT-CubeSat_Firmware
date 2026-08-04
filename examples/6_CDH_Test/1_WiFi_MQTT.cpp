/**
 * VÍ DỤ: KẾT NỐI WIFI VÀ GỬI MQTT (TELEMETRY)
 * 
 * Mô tả:
 * Phân hệ CDH xử lý việc kết nối Internet và giao tiếp với Trạm mặt đất
 * thông qua chuẩn MQTT.
 * - Tự động đọc WiFi từ file `.env`.
 * - Tự động fallback sang chế độ phát WiFi "CubeSat_AP" nếu mất mạng.
 */

#include <Arduino.h>
#include <PTITCube.h>

PTIT_CDH cdh;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\n[EXAMPLE] Khởi tạo CDH (WiFi & MQTT)...");
    
    // Lệnh này sẽ kết nối WiFi và cấu hình MQTT Broker
    cdh.init();
}

void loop() {
    // Duy trì kết nối MQTT và WiFi
    cdh.update();

    static unsigned long lastPub = 0;
    if (millis() - lastPub > 5000) {
        lastPub = millis();

        // Tạo một gói JSON giả
        String payload = "{\"temp\": 25.5, \"batt\": 4.1}";
        
        Serial.print("Đang gửi MQTT: ");
        Serial.println(payload);
        
        // Gửi lên Topic "cubesat/telemetry"
        cdh.publishData("cubesat/telemetry", payload.c_str());
    }
}
