/**
 * @file 1_Capture_To_SD.cpp
 * @brief Ví dụ cơ bản về khởi tạo Camera và chụp ảnh lưu vào thẻ MicroSD.
 * 
 * Mô tả:
 * - Khởi tạo module ESP32-CAM và thẻ nhớ MicroSD.
 * - Sau mỗi 10 giây, camera sẽ chụp một bức ảnh và lưu vào thẻ nhớ với tên 
 *   tuần tự (VD: /photos/image_0.jpg, /photos/image_1.jpg).
 * 
 * Lưu ý: 
 * - Đảm bảo chân CS của thẻ nhớ đã được cấu hình đúng (Mặc định: GPIO 13).
 */

#include <Arduino.h>
#include "MUX/Camera.h"
#include "CDH/Storage.h" // Sử dụng Storage để hỗ trợ kiểm tra file nếu cần

PTIT_Camera camera;
int imageCount = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n--- BẮT ĐẦU TEST CAMERA ---");

    // Khởi tạo Camera và SD card bên trong module
    // Hàm init mặc định: gpsSelectPin = 12, camSelectPin = 14
    if (!camera.init()) {
        Serial.println("Camera: Init failed");
        while (1) {
            delay(1000); // Dừng hệ thống nếu lỗi
        }
    }

    Serial.println("[Thành công] Camera đã sẵn sàng.");
}

void loop() {
    if (camera.isReady()) {
        // Tạo đường dẫn file ảnh
        String path = "/photos/image_" + String(imageCount) + ".jpg";
        
        Serial.print("Đang chụp ảnh và lưu vào: ");
        Serial.println(path);

        // Thực hiện chụp ảnh
        if (camera.captureToFile(path.c_str())) {
            imageCount++;
        } else {
            Serial.println("[Thất bại] Chụp hoặc lưu ảnh bị lỗi.");
        }
    } else {
        Serial.println("[Lỗi] Camera không ở trạng thái sẵn sàng.");
    }

    // Đợi 10 giây trước khi chụp bức tiếp theo
    delay(10000);
}
