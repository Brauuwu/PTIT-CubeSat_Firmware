/**
 * VÍ DỤ: PHÁT HIỆN RƠI TỰ DO (FREEFALL DETECTION)
 * 
 * Mô tả:
 * Khi một vật thể rơi tự do, gia tốc đo được theo tất cả các trục (X, Y, Z) 
 * sẽ tiến gần về 0 (0g). Ví dụ này theo dõi gia tốc trục X từ MPU6050 để 
 * cảnh báo khi vệ tinh hoặc thiết bị đang trong trạng thái không trọng lượng.
 * 
 * Lưu ý: API getAccX() trả về gia tốc theo m/s^2. Trọng lực Trái đất là ~9.81 m/s^2.
 */

#include <Arduino.h>
#include <PTITCube.h>

PTIT_Sensor mySensor;

// Ngưỡng rơi tự do (gần 0). Giá trị lý tưởng là 0, nhưng do nhiễu nên để ngưỡng nhỏ.
const float FREEFALL_THRESHOLD = 2.0; // m/s^2

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\n[EXAMPLE] Bắt đầu bài test Phát hiện Rơi Tự Do...");
    mySensor.init();
}

void loop() {
    mySensor.update();

    // Lấy gia tốc trục X
    float accX = mySensor.getAccX();
    
    // Tính giá trị tuyệt đối
    float absAccX = abs(accX);

    Serial.printf("Gia tốc trục X: %.2f m/s^2", accX);

    if (absAccX < FREEFALL_THRESHOLD) {
        Serial.println(" ---> [CẢNH BÁO] ĐANG RƠI TỰ DO (FREEFALL DETECTED)!");
        
        // Bạn có thể kích hoạt bung dù (Parachute) tại đây!
        // VD: Parachute::deploy();
    } else {
        Serial.println(" ---> Trạng thái bình thường.");
    }

    // Tốc độ quét nhanh để kịp thời phát hiện rơi
    delay(100);
}
