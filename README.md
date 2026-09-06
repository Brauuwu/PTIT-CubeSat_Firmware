# CubeSat Firmware (PTITCube)

Đây là project tổng hợp (Framework) cho CubeSat, tích hợp đầy đủ các phân hệ phần cứng và phần mềm. Hệ thống được thiết kế theo mô hình hướng đối tượng (OOP) kết hợp với hệ điều hành thời gian thực (FreeRTOS) để tối ưu hóa hiệu suất đa nhiệm.

## Sơ đồ kết nối chân (Pinout)

### 1. Phân hệ Cảm biến (I2C)
Dành cho các cảm biến MPU6050 (Gia tốc & Góc quay), BMP280 (Nhiệt độ/Áp suất), GY-271 (La bàn từ trường QMC5883P / HMC5883L):
- **ESP32 SDA** -> Chân 21
- **ESP32 SCL** -> Chân 22

*(Ghi chú: Thư viện tích hợp sẵn cơ chế Auto-Detect để tự động nhận diện địa chỉ I2C của GY-271 là 0x0D hay 0x1E).*

### 2. Phân hệ Lưu trữ MicroSD (SPI)
- **ESP32 SCK** -> Chân 18
- **ESP32 MISO** -> Chân 19
- **ESP32 MOSI** -> Chân 23
- **ESP32 CS** -> Chân 13

### 3. Phân hệ Định vị GPS NEO (UART2)
- **ESP32 TX2 (Chân 17)** -> RX của GPS
- **ESP32 RX2 (Chân 16)** -> TX của GPS

### 4. Phân hệ Viễn thông LoRa E32 (UART1 Remap)
- **ESP32 TX (Chân 25)** -> RX của LoRa
- **ESP32 RX (Chân 26)** -> TX của LoRa
- **ESP32 Chân 32** -> M0 của LoRa
- **ESP32 Chân 33** -> M1 của LoRa
- **ESP32 Chân 27** -> AUX của LoRa

### 5. Phân hệ Năng lượng (EPS)
- **ESP32 Chân 5 (ADC)** -> Nối với cầu phân áp (2 điện trở 1.2k) để đo điện áp Pin (Battery Voltage).

---

## Các thư viện được sử dụng (Dependencies)
Project quản lý thư viện tự động qua PlatformIO (`platformio.ini`). Danh sách bao gồm:

1. **`wnatth3/WiFiManager @ ^2.0.16-rc.2`**: Quản lý cấu hình WiFi (Phát Access Point nếu mất mạng).
2. **`knolleary/PubSubClient @ ^2.8`**: Giao thức MQTT để gửi Telemetry về máy chủ.
3. **`bblanchon/ArduinoJson @ ^7.1.0`**: Đóng gói/giải mã dữ liệu dạng JSON.
4. **`mikalhart/TinyGPSPlus @ ^1.0.3`**: Đọc và bóc tách gói tin chuẩn NMEA từ module GPS.
5. **`adafruit/Adafruit MPU6050 @ ^2.2.6`**: Đọc dữ liệu gia tốc kế & con quay 6 trục MPU6050.
6. **`adafruit/Adafruit BMP280 Library @ ^2.6.8`**: Đọc áp suất và nhiệt độ từ BMP280.
7. **`adafruit/Adafruit QMC5883P Library @ ^1.0.2`**: Đọc la bàn từ trường cho mạch GY-271 (đã vá lỗi I2C).
8. **`xreef/EByte LoRa E32 library @ ^1.5.13`**: Giao tiếp vô tuyến đường dài cho LoRa E32.

---

## Cấu trúc thư mục & Module (PTITCube)
Kiến trúc phần mềm được chia thành các hệ thống con độc lập, được điều phối bởi **PTIT_Scheduler** (dựa trên FreeRTOS):

- `Scheduler`: Trái tim điều phối các tác vụ (Tasks) chạy song song trên 2 Core của ESP32, bảo vệ tài nguyên (Mutex) và chống treo hệ thống (Watchdog Timer).
- `EPS`: Quản lý năng lượng (Nguồn).
- `CDH`: Xử lý trung tâm, WiFi (AP fallback), MQTT.
- `COM`: Viễn thông LoRa E32 (Tích hợp cơ chế cắt nhỏ gói tin Chunking để chống ngợp băng thông LoRa).
- `Sensor`: Các cảm biến (MPU6050, BMP280, GY-271).
- `GPS`: Định vị vệ tinh.
- `Storage`: Lưu trữ thẻ nhớ MicroSD với hệ thống file tựa Linux.
- `Camera`: Module chụp ảnh.
- `examples/`: Chứa các code mẫu chạy thử từng module và các mô hình nâng cao như **Event-Driven**, **Ground Station**, **Multitasking**.

## Tính năng nổi bật

### 1. Trạm mặt đất từ xa (Ground Station)
Bạn có thể giao tiếp với vệ tinh cách xa nhiều kilomet bằng cách truyền các lệnh văn bản thuần qua giao thức LoRa:
- **Lệnh Hệ Thống**: `ping` (kiểm tra đường truyền), `reboot` (khởi động lại), `status` (sức khoẻ pin & hệ thống), `uptime`, `pause` / `resume` (chuyển chế độ Standby tiết kiệm pin).
- **Lệnh Cảm Biến**: `get_gps`, `get_sensor`, `get_eps`.
- **Lệnh Thẻ Nhớ (Linux FS)**: `ls`, `cd`, `pwd`, `mkdir`, `rm`, `touch`, `clear_log`.

### 2. An toàn dữ liệu & Đa nhiệm (FreeRTOS)
Các hệ thống con đọc cảm biến, gửi LoRa, ghi thẻ nhớ chạy độc lập. Nếu vệ tinh đang bận ghi thẻ nhớ hoặc bắn LoRa, FreeRTOS Mutex sẽ khóa (lock) bus I2C/SPI để ngăn rác dữ liệu. Watchdog Timer bảo vệ hệ thống khỏi các vòng lặp vô hạn (Infinite loop) ngoài không gian, giúp ESP32 tự khởi động lại khi có lỗi kẹt logic.

### 3. WiFi & MQTT 
- ESP32 sẽ cố kết nối WiFi cấu hình trong `.env`. 
- Nếu sai mật khẩu hoặc mất mạng, WiFiManager tự động phát WiFi mang tên `CubeSat_AP` (Mật khẩu: `12345678`). Dùng điện thoại kết nối vào mạng này để cấu hình lại mà không cần nạp code!
- Telemetry được phân tách thành từng gói JSON nhỏ gọn (`env`, `eps`, `acc`, `gyro`, `mag`, `gps`) và gửi qua cả LoRa lẫn MQTT định kỳ.

## Cài đặt Cấu Hình Kín (Credentials)
1. Sao chép file `.env_example` thành `.env`
2. Điền thông tin WiFi và MQTT của bạn vào `.env`
3. PlatformIO tự động parse `.env` qua script `env_script.py` và nhúng vào code dưới dạng C++ macros. Trình biên dịch sẽ tự động đọc (không cần nhúng cứng password vào file code cpp).
