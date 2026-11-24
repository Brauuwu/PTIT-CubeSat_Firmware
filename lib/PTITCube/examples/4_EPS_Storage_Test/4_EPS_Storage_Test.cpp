/**
 * VÍ DỤ 4: KIỂM TRA PHÂN HỆ NGUỒN (EPS) VÀ LƯU TRỮ (MICRO SD)
 * 
 * Mô tả:
 * Ví dụ này kết hợp việc đo điện áp của Pin (Li-ion/Li-po) bằng ADC của ESP32,
 * sau đó đóng gói dữ liệu và liên tục ghi log vào thẻ nhớ MicroSD để có thể 
 * phân tích mức tiêu thụ năng lượng của vệ tinh sau khi thu hồi.
 */

#include <Arduino.h>
#include <PTITCube.h>

// KHAI BÁO CÁC ĐỐI TƯỢNG PHÂN HỆ
PTIT_EPS myBattery;
PTIT_Storage myDisk;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\n[EXAMPLE] Bắt đầu bài test Phân hệ Nguồn và Lưu trữ...");

    // Khởi tạo các đối tượng
    myBattery.init();
    myDisk.init();
    
    Serial.println("[EXAMPLE] Bắt đầu ghi log điện áp Pin...\n");
}

void loop() {
    // Đọc điện áp thực tế của Pin thông qua phương thức của đối tượng myBattery
    float voltage = myBattery.getBatteryVoltage();
    
    // Đóng gói thông tin
    String logString = "[Uptime: " + String(millis() / 1000) + "s] ";
    logString += "Battery Voltage: " + String(voltage, 2) + "V";

    // Hiển thị ra màn hình
    Serial.println(logString);

    // Ghi vào thẻ nhớ
    myDisk.logData(logString.c_str());

    // Cảnh báo nếu pin yếu
    if (voltage < 3.2 && voltage > 1.0) {
        Serial.println("[WARNING] Điện áp pin quá thấp. Cần sạc hoặc ngắt các module hao pin!");
        myDisk.logData("[WARNING] Low battery detected!");
    }

    delay(5000);
}
