/**
 * VÍ DỤ 2: CƠ CHẾ BẢO VỆ WATCHDOG TIMER (WDT)
 * 
 * Mô tả:
 * Trên vệ tinh CubeSat, nếu một luồng bị treo (vòng lặp vô hạn, kẹt I2C), 
 * vệ tinh sẽ bị liệt vĩnh viễn ngoài không gian. 
 * Watchdog Timer sẽ phát hiện sự cố này và lập tức khởi động lại (Reset) vi điều khiển.
 */

#include <Arduino.h>
#include <PTITCube.h>

PTIT_Scheduler scheduler;

// Task bình thường
void normalTask() {
    PTIT_Scheduler::println("[Normal] Task này vẫn sống và hoạt động đều đặn.");
}

// Task lỗi: Sẽ bị treo sau 10 giây
void stuckTask() {
    static unsigned long startTime = millis();
    unsigned long elapsed = millis() - startTime;

    if (elapsed > 10000) {
        PTIT_Scheduler::println("\n[ERROR] stuckTask bắt đầu bị kẹt trong vòng lặp vô hạn!");
        // Treo hệ thống (simulate lỗi I2C timeout hoặc lỗi logic)
        while (true) {
            // Không bao giờ thoát khỏi đây!
            delay(10); 
        }
    } else {
        PTIT_Scheduler::println("[StuckTask] Còn " + String(10 - elapsed/1000) + " giây nữa là bị treo.");
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\n====================================");
    Serial.println("[EXAMPLE] Khởi động Watchdog Test!");
    Serial.println("Sau 10 giây, task lỗi sẽ treo hệ thống.");
    Serial.println("Watchdog được hẹn giờ 5 giây. Đợi xem ESP32 có tự Reset không nhé!");
    Serial.println("====================================\n");

    scheduler.addTask("Normal", normalTask, 1000, 1, 2);
    scheduler.addTask("Stuck",  stuckTask,  1000, 1, 2);

    // Kích hoạt Watchdog với giới hạn 5 giây
    // Nếu bất kỳ task nào tốn > 5 giây để thực thi, ESP32 sẽ bị Reset ngay lập tức.
    scheduler.enableWatchdog(5);

    scheduler.start();
}

void loop() {
    // Để trống
}
