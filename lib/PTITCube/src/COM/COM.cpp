#include "COM.h"

PTIT_COM::PTIT_COM() : e32(&Serial1, 27, 32, 33, UART_BPS_RATE_9600) {
}

void PTIT_COM::init() {
    Serial.println("[COM] Initializing LoRa E32 (UART1)...");
    Serial1.begin(9600, SERIAL_8N1, 26, 25); // RX=26, TX=25
    e32.begin();
    ResponseStructContainer c = e32.getConfiguration();
    if (c.status.code == E32_SUCCESS) {
        Configuration config = *(Configuration *)c.data;
        config.ADDH = 0x00;
        config.ADDL = 0x00;
        config.CHAN = 0x17;
        e32.setConfiguration(config, WRITE_CFG_PWR_DWN_SAVE);
        Serial.println("[COM] LoRa E32 OK");
    } else {
        Serial.println("[COM] LoRa config failed");
    }
}

String PTIT_COM::update() {
    if (e32.available() > 1) {
        ResponseContainer rs = e32.receiveMessage();
        String message = rs.data;
        Serial.print("[COM] Nhận được tin nhắn LoRa: ");
        Serial.println(message);
        return message;
    }
    return "";
}

void PTIT_COM::sendPing() {
    sendMessage("Ping");
}

void PTIT_COM::sendMessage(String msg) {
    if (!msg.endsWith("\n")) {
        msg += "\n";
    }
    
    int maxLen = 50; // Kích thước gói tin an toàn cho LoRa E32
    int len = msg.length();
    
    if (len <= maxLen) {
        ResponseStatus rs = e32.sendMessage(msg);
        Serial.print("[COM] Đã gửi tin nhắn: ");
        Serial.print(msg);
        Serial.print(" (Mã lỗi: ");
        Serial.print(rs.code);
        Serial.println(")");
    } else {
        Serial.println("[COM] Tin nhắn quá dài (" + String(len) + " bytes), đang chia nhỏ...");
        for (int i = 0; i < len; i += maxLen) {
            String chunk = msg.substring(i, i + maxLen);
            ResponseStatus rs = e32.sendMessage(chunk);
            Serial.print("[COM] Đã gửi chunk: ");
            Serial.print(chunk);
            Serial.print(" (Mã lỗi: ");
            Serial.print(rs.code);
            Serial.println(")");
            delay(200); // Chờ 200ms giữa các chunk để module LoRa kịp phát
        }
    }
}

bool PTIT_COM::setChannel(uint8_t channel) {
    ResponseStructContainer c = e32.getConfiguration();
    if (c.status.code != E32_SUCCESS) return false;
    Configuration config = *(Configuration *)c.data;
    config.CHAN = channel;
    ResponseStatus rs = e32.setConfiguration(config, WRITE_CFG_PWR_DWN_SAVE);
    return (rs.code == E32_SUCCESS);
}

bool PTIT_COM::setAddress(uint8_t addh, uint8_t addl) {
    ResponseStructContainer c = e32.getConfiguration();
    if (c.status.code != E32_SUCCESS) return false;
    Configuration config = *(Configuration *)c.data;
    config.ADDH = addh;
    config.ADDL = addl;
    ResponseStatus rs = e32.setConfiguration(config, WRITE_CFG_PWR_DWN_SAVE);
    return (rs.code == E32_SUCCESS);
}

bool PTIT_COM::setTransmissionMode(uint8_t mode) {
    ResponseStructContainer c = e32.getConfiguration();
    if (c.status.code != E32_SUCCESS) return false;
    Configuration config = *(Configuration *)c.data;
    // mode: 0 = Transparent, 1 = Fixed Transmission
    config.OPTION.fixedTransmission = mode;
    ResponseStatus rs = e32.setConfiguration(config, WRITE_CFG_PWR_DWN_SAVE);
    return (rs.code == E32_SUCCESS);
}
