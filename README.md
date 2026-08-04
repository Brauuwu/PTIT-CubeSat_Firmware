# CubeSat Firmware

Đây là project tổng hợp (Framework) cho CubeSat, tích hợp đầy đủ các phân hệ.

## Sơ đồ kết nối chân (Pinout)

### 1. Phân hệ Cảm biến (I2C)
Dành cho các cảm biến MPU6050 (Gia tốc), BMP280 (Nhiệt độ/Áp suất), GY-271 / QMC5883L (La bàn):
- **ESP32 SDA** -> Chân 21
- **ESP32 SCL** -> Chân 22

### 2. Phân hệ Lưu trữ MicroSD (SPI)
- **ESP32 SCK** -> Chân 18
- **ESP32 MISO** -> Chân 19
- **ESP32 MOSI** -> Chân 23
- **ESP32 CS** -> Chân 15

### 3. Phân hệ Định vị GPS NEO (UART2)
- **ESP32 TX2 (Chân 17)** -> RX của GPS
- **ESP32 RX2 (Chân 16)** -> TX của GPS
- **ESP32 Chân 4** -> Xung PPS của GPS

### 4. Phân hệ Viễn thông LoRa E32 (UART1 Remap)
- **ESP32 TX (Chân 25)** -> RX của LoRa
- **ESP32 RX (Chân 26)** -> TX của LoRa
- **ESP32 Chân 32** -> M0 của LoRa
- **ESP32 Chân 33** -> M1 của LoRa
- **ESP32 Chân 27** -> AUX của LoRa

### 5. Phân hệ Năng lượng (EPS)
- **ESP32 Chân 34 (ADC)** -> Nối với cầu phân áp (2 điện trở 100k) để đo điện áp Pin (Battery Voltage).

---

## Các thư viện được sử dụng (Dependencies)
Project quản lý thư viện tự động qua PlatformIO (`platformio.ini`). Danh sách bao gồm:

1. **`wnatth3/WiFiManager @ ^2.0.16-rc.2`**: Quản lý cấu hình WiFi (Phát Access Point nếu mất mạng).
2. **`knolleary/PubSubClient @ ^2.8`**: Giao thức MQTT để gửi Telemetry về máy chủ mặt đất.
3. **`bblanchon/ArduinoJson @ ^7.1.0`**: Đóng gói/giải mã dữ liệu dạng JSON.
4. **`mikalhart/TinyGPSPlus @ ^1.0.3`**: Đọc và bóc tách gói tin chuẩn NMEA từ module GPS.
5. **`adafruit/Adafruit MPU6050 @ ^2.2.6`**: Đọc dữ liệu gia tốc kế 6 trục MPU6050.
6. **`adafruit/Adafruit BMP280 Library @ ^2.6.8`**: Đọc áp suất và nhiệt độ từ BMP280.
7. **`mprograms/QMC5883LCompass @ ^1.2.3`**: Đọc la bàn từ trường cho GY-271 (sử dụng chip QMC5883L).
8. **`xreef/EByte LoRa E32 library @ ^1.5.13`**: Giao tiếp, cấu hình (địa chỉ, kênh tần số) và truyền nhận gói tin cho LoRa E32.

---

## Cấu trúc thư mục mã nguồn
- `lib/PTITCube/src/EPS`: Quản lý năng lượng (Nguồn)
- `lib/PTITCube/src/CDH`: Xử lý trung tâm, WiFi (AP fallback), MQTT
- `lib/PTITCube/src/COM`: Viễn thông LoRa E32
- `lib/PTITCube/src/Sensor`: Các cảm biến (MPU6050, BMP280, GY-271)
- `lib/PTITCube/src/GPS`: Định vị vệ tinh
- `lib/PTITCube/src/Storage`: Lưu trữ thẻ nhớ MicroSD
- `lib/PTITCube/src/Camera`: Module chụp ảnh
- `examples/`: Chứa các code mẫu (ví dụ) chạy riêng lẻ cho từng module trên để test rời.

## Cài đặt Credentials (Bảo mật thông tin)
1. Sao chép file `.env_example` thành `.env`
2. Điền thông tin WiFi và MQTT của bạn vào `.env`
3. PlatformIO sẽ tự động parse `.env` nhờ script `env_script.py` và nhúng vào code dưới dạng C++ macros (Ví dụ: `WIFI_SSID`, `MQTT_HOST`). Bạn không cần sửa thẳng code.

## Tính năng WiFi & MQTT
- ESP32 sẽ cố gắng kết nối WiFi trong file `.env`. 
- Nếu không có mạng hoặc sai mật khẩu, `WiFiManager` sẽ tự động tạo một mạng WiFi Access Point có tên là `CubeSat_AP` (Mật khẩu mặc định: `12345678`). Bạn có thể dùng điện thoại kết nối vào mạng này để cấu hình WiFi mới cho mạch mà không cần nạp lại code!
- Dữ liệu (Telemetry) được đóng gói JSON và gửi lên broker MQTT định kỳ, đồng thời ghi log vào MicroSD và bắn qua LoRa dự phòng.
