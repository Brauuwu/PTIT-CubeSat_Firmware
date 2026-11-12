const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

const exportDir = 'e:\\cube\\CubeSat_Labs_Export';
const sourceTemplate = 'e:\\cube\\CubeSat-Firmware\\platformio.ini';

// 1. Cập nhật platformio.ini
function cleanPlatformioIni(filePath) {
    if (!fs.existsSync(filePath)) return;
    let content = fs.readFileSync(filePath, 'utf8');
    
    // Tìm vị trí của [env:esp32cam]
    const camEnvIndex = content.indexOf('[env:esp32cam]');
    if (camEnvIndex !== -1) {
        content = content.substring(0, camEnvIndex).trim();
    }
    
    // Xóa build_src_filter = +<main.cpp> -<esp32cam_main.cpp>
    content = content.replace(/build_src_filter\s*=\s*\+<main\.cpp>\s*-<esp32cam_main\.cpp>\r?\n?/g, '');
    
    fs.writeFileSync(filePath, content + '\n');
}

// Cập nhật ở bản gốc
cleanPlatformioIni(sourceTemplate);

// Cập nhật ở các lab
if (fs.existsSync(exportDir)) {
    const folders = fs.readdirSync(exportDir);
    folders.forEach(folder => {
        const pioPath = path.join(exportDir, folder, 'platformio.ini');
        cleanPlatformioIni(pioPath);
        
        // Cập nhật git
        try {
            const repoPath = path.join(exportDir, folder);
            if (fs.existsSync(path.join(repoPath, '.git'))) {
                execSync('git add platformio.ini', { cwd: repoPath, stdio: 'ignore' });
                execSync('git commit -m "Remove esp32cam env"', { cwd: repoPath, stdio: 'ignore' });
            }
        } catch (e) {
            // Ignore if nothing to commit
        }
    });
}

// 2. Tạo example đặc biệt cho ESP32-CAM
const camRepoName = 'ESP32_CAM_Firmware';
const camRepoDir = path.join(exportDir, camRepoName);

if (fs.existsSync(camRepoDir)) {
    fs.rmSync(camRepoDir, { recursive: true, force: true });
}
fs.mkdirSync(camRepoDir, { recursive: true });

// Viết platformio.ini riêng cho ESP32-CAM
const camPioContent = `; PlatformIO Project Configuration File cho ESP32-CAM

[env:esp32cam]
platform = espressif32
board = esp32cam
framework = arduino
monitor_speed = 115200
lib_deps =
    https://github.com/espressif/esp32-camera.git
`;
fs.writeFileSync(path.join(camRepoDir, 'platformio.ini'), camPioContent);

// Tạo src/main.cpp cho ESP32-CAM
const srcDir = path.join(camRepoDir, 'src');
fs.mkdirSync(srcDir);
const camCode = `
#include <Arduino.h>
#include "esp_camera.h"

// Cấu hình chân cho module AI Thinker ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup() {
    Serial.begin(115200);
    Serial.println("Khoi dong ESP32-CAM...");

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;

    // Khởi tạo camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\\n", err);
        return;
    }
    
    Serial.println("Camera init OK.");
    Serial.println("Sẵn sàng nhận lệnh 'CAPTURE' từ ESP32 OBC...");
}

void loop() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\\n');
        cmd.trim();
        
        if (cmd == "CAPTURE") {
            Serial.println("Dang chup anh...");
            camera_fb_t * fb = esp_camera_fb_get();
            if (!fb) {
                Serial.println("Camera capture failed");
                return;
            }
            
            Serial.printf("Chup anh thanh cong! Kich thuoc: %d bytes\\n", fb->len);
            
            // Xử lý gửi ảnh qua Serial (nếu cần thiết) hoặc báo tín hiệu
            // Ở ví dụ này ta chỉ báo hiệu
            Serial.println("DONE");
            
            esp_camera_fb_return(fb);
        }
    }
}
`;
fs.writeFileSync(path.join(srcDir, 'main.cpp'), camCode.trim());

// Create README
fs.writeFileSync(path.join(camRepoDir, 'README.md'), '# ' + camRepoName + '\n\n**Firmware độc lập cho module ESP32-CAM**\n\nFirmware này chỉ dành riêng cho bo mạch ESP32-CAM. Nó đóng vai trò như một cảm biến/camera slave, giao tiếp với máy tính trung tâm (ESP32 Dev Module / OBC) qua giao tiếp UART (Serial).\n\nKhi OBC gửi lệnh `CAPTURE` qua cổng Serial, ESP32-CAM sẽ chụp một bức ảnh và trả về tín hiệu kết quả.\n');

// Copy .gitignore
if (fs.existsSync('e:\\cube\\CubeSat-Firmware\\.gitignore')) {
    fs.copyFileSync('e:\\cube\\CubeSat-Firmware\\.gitignore', path.join(camRepoDir, '.gitignore'));
}

// Git init
try {
    execSync('git init', { cwd: camRepoDir, stdio: 'ignore' });
    execSync('git add .', { cwd: camRepoDir, stdio: 'ignore' });
    execSync('git commit -m "Initial commit ESP32-CAM firmware"', { cwd: camRepoDir, stdio: 'ignore' });
} catch (e) {
    console.error(e.message);
}

console.log("Hoàn tất cập nhật platformio.ini và tạo ESP32_CAM_Firmware!");
