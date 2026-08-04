/**
 * VÍ DỤ 3: KIỂM TRA PHÂN HỆ VIỄN THÔNG (LORA E32)
 * 
 * Mô tả:
 * Ví dụ này hướng dẫn cách cấu hình nâng cao và truyền/nhận dữ liệu bằng sóng 
 * vô tuyến sử dụng module LoRa E32 từ Ebyte.
 * 
 * Chú ý đấu nối phần cứng (UART1 + GPIOs điều khiển):
 * - Chân TX của LoRa  -> Chân 26 (RX của ESP32).
 * - Chân RX của LoRa  -> Chân 25 (TX của ESP32).
 * - Chân M0 của LoRa  -> Chân 32.
 * - Chân M1 của LoRa  -> Chân 33.
 * - Chân AUX của LoRa -> Chân 27.
 */

#include <Arduino.h>
#include <PTITCube.h>

// KHAI BÁO ĐỐI TƯỢNG LORA
PTIT_COM myLoRa;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\n[EXAMPLE] Bắt đầu bài test Phân hệ Viễn thông vô tuyến (LoRa)...");

    // Khởi tạo
    myLoRa.init();

    // ---------------- CẤU HÌNH LORA NÂNG CAO ----------------
    // Tại đây bạn có thể sử dụng các API mới để thay đổi tham số vật lý của sóng LoRa.
    // Sử dụng số THẬP PHÂN (Decimal) để cấu hình dễ dàng hơn.
    
    // 1. Thay đổi Kênh (Channel) - Quyết định tần số phát sóng.
    // Tần số hoạt động = Tần số cơ sở (Vd: 410MHz) + Channel (Vd: 24)
    if (myLoRa.setChannel(24)) {
        Serial.println("[CONFIG] Đã đổi kênh LoRa sang 24 thành công.");
    }

    // 2. Thay đổi Địa chỉ (Address) - Mạng LoRa có thể chia thành nhiều nhóm.
    // Địa chỉ gồm 2 byte: ADDH (High) và ADDL (Low). Các mạch muốn nói chuyện 
    // được với nhau cần cấu hình cùng kênh và cùng địa chỉ (nếu dùng Fixed Transmission).
    // Ví dụ: Địa chỉ High = 0, Low = 1
    if (myLoRa.setAddress(0, 1)) {
        Serial.println("[CONFIG] Đã đổi địa chỉ LoRa thành 0, 1.");
    }

    // 3. Thay đổi chế độ truyền (Transmission Mode)
    // - Mode 0 (Transparent): Module phát cái gì, module thu nhận y hệt cái đó (Broadcast).
    // - Mode 1 (Fixed): Gói tin phải chỉ định rõ Address và Channel của người nhận ở 3 byte đầu.
    if (myLoRa.setTransmissionMode(0)) {
        Serial.println("[CONFIG] Đã chuyển sang chế độ truyền trong suốt (Transparent).");
    }

    Serial.println("\n[EXAMPLE] Bắt đầu vào chế độ Thu/Phát tin nhắn...\n");
}

void loop() {
    // 1. Lắng nghe tin nhắn
    myLoRa.update();

    // 2. Gửi Ping mỗi 5 giây
    static unsigned long lastPing = 0;
    if (millis() - lastPing > 5000) {
        lastPing = millis();
        String myMessage = "Hello from CubeSat ESP32! Uptime: " + String(millis() / 1000) + "s";
        myLoRa.sendMessage(myMessage);
    }
}
