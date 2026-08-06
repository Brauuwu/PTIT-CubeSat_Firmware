#include "COM.h"

#if USE_AS32_MODULE
PTIT_COM::PTIT_COM() : as32(&Serial1, 27, 32, 33, 26, 25) {
}
#else
PTIT_COM::PTIT_COM() : e32(&Serial1, 27, 32, 33, UART_BPS_RATE_9600) {
}
#endif

void PTIT_COM::init() {
#if USE_AS32_MODULE
    Serial.println("[COM] Initializing LoRa AS32 (UART1)...");
    as32.begin();
    
    CFGstruct config;
    if(as32.SleepModeCmd(R_CFG, &config) == RET_SUCCESS) {
        config.ADDH = 0x00;
        config.ADDL = 0x00;
        config.CHAN = 0x17;
        config.OPTION_bits.trsm_mode = TRSM_FP_MODE;
        config.OPTION_bits.tsmt_pwr = TSMT_PWR_20DB;
        as32.SleepModeCmd(W_CFG_PWR_DWN_SAVE, &config);
        Serial.println("[COM] LoRa AS32 OK");
    } else {
        Serial.println("[COM] LoRa AS32 config failed");
    }
#else
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
#endif
}

String PTIT_COM::update() {
#if USE_AS32_MODULE
    if (as32.available() > 1) {
        String message = as32.receiveMessage();
        Serial.print("[COM] Nhận được tin nhắn LoRa AS32: ");
        Serial.println(message);
        return message;
    }
#else
    if (e32.available() > 1) {
        ResponseContainer rs = e32.receiveMessage();
        String message = rs.data;
        Serial.print("[COM] Nhận được tin nhắn LoRa E32: ");
        Serial.println(message);
        return message;
    }
#endif
    return "";
}

void PTIT_COM::sendPing() {
    sendMessage("Ping");
}

void PTIT_COM::sendMessage(String msg) {
    if (!msg.endsWith("\n")) {
        msg += "\n";
    }
    
    int maxLen = 50; 
    int len = msg.length();
    
    if (len <= maxLen) {
#if USE_AS32_MODULE
        RET_STATUS rs = as32.sendMessage(msg);
        Serial.print("[COM] Đã gửi tin nhắn (AS32): ");
        Serial.print(msg);
        Serial.print(" (Mã lỗi: ");
        Serial.print(rs);
        Serial.println(")");
#else
        ResponseStatus rs = e32.sendMessage(msg);
        Serial.print("[COM] Đã gửi tin nhắn (E32): ");
        Serial.print(msg);
        Serial.print(" (Mã lỗi: ");
        Serial.print(rs.code);
        Serial.println(")");
#endif
    } else {
        Serial.println("[COM] Tin nhắn quá dài (" + String(len) + " bytes), đang chia nhỏ...");
        for (int i = 0; i < len; i += maxLen) {
            String chunk = msg.substring(i, i + maxLen);
#if USE_AS32_MODULE
            RET_STATUS rs = as32.sendMessage(chunk);
            Serial.print("[COM] Đã gửi chunk (AS32): ");
            Serial.print(chunk);
            Serial.print(" (Mã lỗi: ");
            Serial.print(rs);
            Serial.println(")");
#else
            ResponseStatus rs = e32.sendMessage(chunk);
            Serial.print("[COM] Đã gửi chunk (E32): ");
            Serial.print(chunk);
            Serial.print(" (Mã lỗi: ");
            Serial.print(rs.code);
            Serial.println(")");
#endif
            delay(200);
        }
    }
}

bool PTIT_COM::setChannel(uint8_t channel) {
#if USE_AS32_MODULE
    CFGstruct config;
    if(as32.SleepModeCmd(R_CFG, &config) != RET_SUCCESS) return false;
    config.CHAN = channel;
    return (as32.SleepModeCmd(W_CFG_PWR_DWN_SAVE, &config) == RET_SUCCESS);
#else
    ResponseStructContainer c = e32.getConfiguration();
    if (c.status.code != E32_SUCCESS) return false;
    Configuration config = *(Configuration *)c.data;
    config.CHAN = channel;
    ResponseStatus rs = e32.setConfiguration(config, WRITE_CFG_PWR_DWN_SAVE);
    return (rs.code == E32_SUCCESS);
#endif
}

bool PTIT_COM::setAddress(uint8_t addh, uint8_t addl) {
#if USE_AS32_MODULE
    CFGstruct config;
    if(as32.SleepModeCmd(R_CFG, &config) != RET_SUCCESS) return false;
    config.ADDH = addh;
    config.ADDL = addl;
    return (as32.SleepModeCmd(W_CFG_PWR_DWN_SAVE, &config) == RET_SUCCESS);
#else
    ResponseStructContainer c = e32.getConfiguration();
    if (c.status.code != E32_SUCCESS) return false;
    Configuration config = *(Configuration *)c.data;
    config.ADDH = addh;
    config.ADDL = addl;
    ResponseStatus rs = e32.setConfiguration(config, WRITE_CFG_PWR_DWN_SAVE);
    return (rs.code == E32_SUCCESS);
#endif
}

bool PTIT_COM::setTransmissionMode(uint8_t mode) {
#if USE_AS32_MODULE
    CFGstruct config;
    if(as32.SleepModeCmd(R_CFG, &config) != RET_SUCCESS) return false;
    config.OPTION_bits.trsm_mode = mode;
    return (as32.SleepModeCmd(W_CFG_PWR_DWN_SAVE, &config) == RET_SUCCESS);
#else
    ResponseStructContainer c = e32.getConfiguration();
    if (c.status.code != E32_SUCCESS) return false;
    Configuration config = *(Configuration *)c.data;
    config.OPTION.fixedTransmission = mode;
    ResponseStatus rs = e32.setConfiguration(config, WRITE_CFG_PWR_DWN_SAVE);
    return (rs.code == E32_SUCCESS);
#endif
}
