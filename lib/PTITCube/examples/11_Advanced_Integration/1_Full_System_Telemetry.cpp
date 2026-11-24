/**
 * @file 1_Full_System_Telemetry.cpp
 * @brief Ví dụ tổng hợp toàn bộ các phân hệ của vệ tinh CubeSat.
 * 
 * Mô tả:
 * - Khởi tạo toàn bộ các module: EPS (Nguồn), Sensor (Gia tốc, Góc quay, Áp suất, Từ trường),
 *   GPS (Định vị), Camera (Hình ảnh), Storage (Thẻ MicroSD), COM (LoRa).
 * - Thu thập dữ liệu từ tất cả các cảm biến.
 * - Đóng gói dữ liệu thành định dạng JSON.
 * - Gửi chuỗi JSON về trạm mặt đất qua LoRa và lưu vào thẻ nhớ.
 * 
 * Lưu ý:
 * - Trong môi trường thực tế, tác vụ này nên được chạy đa luồng bằng FreeRTOS 
 *   như bản firmware đầy đủ để tránh block hệ thống, nhưng trong ví dụ này
 *   ta dùng vòng lặp tuần tự (Sequential loop) cho dễ hiểu.
 */

#include <Arduino.h>
#include <ArduinoJson.h>

#include "EPS/EPS.h"
#include "Sensor/Sensor.h"
#include "MUX/GPS.h"
#include "MUX/Camera.h"
#include "CDH/Storage.h"
#include "COM/COM.h"

// Khởi tạo các phân hệ
PTIT_EPS eps;
PTIT_Sensor sensor;
PTIT_GPS gps;
PTIT_Camera camera;
PTIT_Storage storage;
PTIT_COM com;

unsigned long lastTelemetryTime = 0;
const unsigned long TELEMETRY_INTERVAL = 5000; // 5 giây gửi telemetry 1 lần

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n--- KHỞI TẠO HỆ THỐNG VỆ TINH TOÀN DIỆN ---");

    // Khởi tạo các phân hệ
    Serial.print("1. Khởi tạo EPS... ");
    eps.init();
    Serial.println("OK");

    Serial.print("2. Khởi tạo Sensor... ");
    sensor.init();
    Serial.println("OK");

    Serial.print("3. Khởi tạo GPS... ");
    gps.init();
    Serial.println("OK");

    Serial.print("4. Khởi tạo Storage (MicroSD)... ");
    storage.init();
    Serial.println("OK");

    Serial.print("5. Khởi tạo Camera... ");
    camera.init();
    Serial.println("OK");

    Serial.print("6. Khởi tạo COM (LoRa)... ");
    com.init();
    Serial.println("OK");

    Serial.println("\n--- VỆ TINH ĐÃ SẴN SÀNG ---");
}

void loop() {
    // 1. Cập nhật dữ liệu từ các cảm biến định kì
    eps.update();
    sensor.update();
    
    // Bật kênh GPS để lấy vị trí
    camera.selectGPS();
    gps.update();

    unsigned long currentMillis = millis();

    // 2. Gửi Telemetry sau mỗi khoảng thời gian định trước
    if (currentMillis - lastTelemetryTime >= TELEMETRY_INTERVAL) {
        lastTelemetryTime = currentMillis;
        
        Serial.println("\n--- ĐANG ĐÓNG GÓI TELEMETRY ---");

        // Tạo Document JSON
        StaticJsonDocument<512> doc;

        // Dữ liệu Nguồn (EPS)
        JsonObject epsObj = doc.createNestedObject("eps");
        epsObj["batt_v"] = eps.getBatteryVoltage();
        epsObj["batt_pct"] = eps.getBatteryPercentage();
        epsObj["sys_v"] = eps.getSystemVoltage();

        // Dữ liệu Cảm biến môi trường (Sensor)
        JsonObject envObj = doc.createNestedObject("env");
        envObj["temp"] = sensor.getTemperature();
        envObj["press"] = sensor.getPressure();
        envObj["alt"] = sensor.getAltitude();

        // Dữ liệu Tư thế (Attitude - IMU)
        JsonObject imuObj = doc.createNestedObject("imu");
        imuObj["roll"] = sensor.getRoll();
        imuObj["pitch"] = sensor.getPitch();
        imuObj["yaw"] = sensor.getYaw();

        // Dữ liệu Định vị (GPS)
        if (gps.hasFix()) {
            JsonObject gpsObj = doc.createNestedObject("gps");
            gpsObj["lat"] = gps.getLatitude();
            gpsObj["lng"] = gps.getLongitude();
            gpsObj["alt"] = gps.getAltitude();
            gpsObj["sats"] = gps.getSatellites();
        }

        // Chuyển đổi JSON thành chuỗi (String)
        String telemetryString;
        serializeJson(doc, telemetryString);

        Serial.println("Dữ liệu thu thập được:");
        Serial.println(telemetryString);

        // 3. Gửi dữ liệu qua LoRa về Trái Đất
        Serial.print("Đang gửi qua LoRa... ");
        if (com.transmitData(telemetryString)) {
            Serial.println("Thành công.");
        } else {
            Serial.println("Thất bại.");
        }

        // 4. Lưu log vào thẻ nhớ làm "Hộp đen" dự phòng
        Serial.print("Đang lưu log vào thẻ nhớ... ");
        // Đảm bảo dạng text có dấu xuống dòng
        telemetryString += "\n"; 
        storage.logData(telemetryString.c_str());
        Serial.println("Đã ghi.");
    }
}
