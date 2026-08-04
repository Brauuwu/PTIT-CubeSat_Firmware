/**
 * VÍ DỤ: ƯỚC TÍNH ĐỘ CAO DỰA VÀO ÁP SUẤT (ALTITUDE ESTIMATOR)
 * 
 * Mô tả:
 * Ví dụ này sử dụng cảm biến BMP280 để đo áp suất khí quyển, sau đó 
 * áp dụng Công thức Khí áp học (Barometric Formula) để ước tính độ cao
 * tương đối so với mặt nước biển (hoặc điểm xuất phát).
 */

#include <Arduino.h>
#include <PTITCube.h>
#include <math.h>

PTIT_Sensor mySensor;

// Áp suất chuẩn tại mặt nước biển (thay đổi theo thời tiết trong ngày, mặc định 1013.25 hPa)
const float SEA_LEVEL_PRESSURE_HPA = 1013.25;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\n[EXAMPLE] Bắt đầu bài test Ước tính Độ cao...");
    mySensor.init();
}

void loop() {
    mySensor.update();

    float pressure = mySensor.getPressure();
    float tempC = mySensor.getTemperature();

    // Công thức tính độ cao từ áp suất:
    // H = 44330 * [1 - (P / P0)^(1/5.255)]
    if (pressure > 0) {
        float altitude = 44330.0 * (1.0 - pow(pressure / SEA_LEVEL_PRESSURE_HPA, 0.1903));
        
        Serial.println("==================================");
        Serial.printf("Nhiệt độ hiện tại: %.2f °C\n", tempC);
        Serial.printf("Áp suất hiện tại : %.2f hPa\n", pressure);
        Serial.printf("=> ĐỘ CAO ƯỚC TÍNH: %.2f mét\n", altitude);
        Serial.println("==================================\n");
    } else {
        Serial.println("[ERROR] Lỗi đọc cảm biến áp suất!");
    }

    delay(2000);
}
