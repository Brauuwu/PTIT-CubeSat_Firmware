/**
 * VÍ DỤ: TÍNH KHOẢNG CÁCH ĐẾN MỤC TIÊU BẰNG GPS
 * 
 * Mô tả:
 * Tính toán khoảng cách đường chim bay từ vị trí hiện tại của CubeSat
 * đến một trạm mặt đất mục tiêu bằng công thức Haversine.
 */

#include <Arduino.h>
#include <PTITCube.h>
#include <math.h>

PTIT_GPS myGPS;

// Tọa độ Trạm mặt đất (Ví dụ: Học viện Công nghệ Bưu chính Viễn thông)
const float TARGET_LAT = 20.980838;
const float TARGET_LNG = 105.787401;
const float EARTH_RADIUS_KM = 6371.0;

// Hàm chuyển đổi Độ sang Radian
float toRadian(float degree) {
    return degree * PI / 180.0;
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\n[EXAMPLE] Bắt đầu bài test Tính Khoảng cách GPS...");
    myGPS.init();
}

void loop() {
    myGPS.update();

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 5000) {
        lastPrint = millis();
        
        float currentLat = myGPS.getLat();
        float currentLng = myGPS.getLng();

        if (currentLat == 0.0 || myGPS.getSatellites() == 0) {
            Serial.println("Đang chờ tín hiệu GPS...");
            return;
        }

        // Tính khoảng cách Haversine
        float dLat = toRadian(TARGET_LAT - currentLat);
        float dLng = toRadian(TARGET_LNG - currentLng);

        float a = sin(dLat/2) * sin(dLat/2) +
                  cos(toRadian(currentLat)) * cos(toRadian(TARGET_LAT)) *
                  sin(dLng/2) * sin(dLng/2);
        
        float c = 2 * atan2(sqrt(a), sqrt(1-a));
        float distanceKM = EARTH_RADIUS_KM * c;

        Serial.println("=============================================");
        Serial.printf("Vị trí hiện tại : %f, %f\n", currentLat, currentLng);
        Serial.printf("Vị trí mục tiêu : %f, %f\n", TARGET_LAT, TARGET_LNG);
        Serial.printf("Khoảng cách     : %.3f KM\n", distanceKM);
        Serial.println("=============================================");
    }
}
