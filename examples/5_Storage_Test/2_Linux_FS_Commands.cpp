/**
 * VÍ DỤ 2: QUẢN LÝ THẺ NHỚ BẰNG LỆNH KIỂU LINUX
 * 
 * Mô tả:
 * Ví dụ này mô phỏng cách nhận lệnh từ một cổng Serial (ví dụ Ground Station)
 * và chuyển nó cho Storage module xử lý như một file system của Linux.
 *
 * Các lệnh hỗ trợ: pwd, cd, ls, mkdir, rm, touch
 */

#include <Arduino.h>
#include <PTITCube.h>

PTIT_Storage storage;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\n[EXAMPLE] Khởi động hệ thống lưu trữ...");
    storage.init();
    
    if (storage.isReady()) {
        Serial.println("[EXAMPLE] SD Card đã sẵn sàng!");
        Serial.println("Bạn có thể gõ các lệnh sau vào ô Serial Monitor (kèm Enter/NL):");
        Serial.println("  pwd      - Xem thư mục hiện tại");
        Serial.println("  ls       - Liệt kê file và thư mục");
        Serial.println("  mkdir x  - Tạo thư mục tên x");
        Serial.println("  cd x     - Chuyển vào thư mục x");
        Serial.println("  touch x  - Tạo file x");
        Serial.println("  rm x     - Xóa file hoặc thư mục x");
    } else {
        Serial.println("[EXAMPLE] LỖI: Không tìm thấy thẻ SD.");
    }
}

void loop() {
    // Đọc lệnh từ Serial Monitor (hoặc từ LoRa trong thực tế)
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if (cmd.length() > 0) {
            Serial.print("User> ");
            Serial.println(cmd);
            
            // Xử lý lệnh
            String response = storage.handleCommand(cmd);
            
            if (response == "") {
                Serial.println("Lệnh không hợp lệ.");
            } else {
                Serial.println(response);
            }
            Serial.println("---------------------------------");
        }
    }
}
