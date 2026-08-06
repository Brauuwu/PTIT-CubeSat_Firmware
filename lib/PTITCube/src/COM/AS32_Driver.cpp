#include "AS32_Driver.h"

AS32_Driver::AS32_Driver(HardwareSerial* serial, int auxPin, int m0Pin, int m1Pin, int rxPin, int txPin) {
    _serial = serial;
    _auxPin = auxPin;
    _m0Pin = m0Pin;
    _m1Pin = m1Pin;
    _rxPin = rxPin;
    _txPin = txPin;
}

void AS32_Driver::begin() {
    pinMode(_m0Pin, OUTPUT);
    pinMode(_m1Pin, OUTPUT);
    pinMode(_auxPin, INPUT);
    _serial->begin(9600, SERIAL_8N1, _rxPin, _txPin);
    
    // Default to Normal Mode
    SwitchMode(MODE_0_NORMAL);
    WaitAUX_H();
}

bool AS32_Driver::ReadAUX() {
    // E32/AS32 AUX is active LOW when busy
    return digitalRead(_auxPin);
}

RET_STATUS AS32_Driver::WaitAUX_H() {
    RET_STATUS STATUS = RET_SUCCESS;
    uint8_t cnt = 0;
    while((ReadAUX() == LOW) && (cnt++ < 100)) {
        delay(10);
    }
    if(cnt >= 100) {
        STATUS = RET_TIMEOUT;
    }
    return STATUS;
}

bool AS32_Driver::chkModeSame(MODE_TYPE mode) {
    static MODE_TYPE pre_mode = MODE_INIT;
    if(pre_mode == mode) {
        return true;
    } else {
        pre_mode = mode;
        return false;
    }
}

void AS32_Driver::SwitchMode(MODE_TYPE mode) {
    if(!chkModeSame(mode)) {
        WaitAUX_H();
        switch (mode) {
            case MODE_0_NORMAL:
                digitalWrite(_m0Pin, LOW);
                digitalWrite(_m1Pin, LOW);
                break;
            case MODE_1_WAKE_UP:
                digitalWrite(_m0Pin, HIGH);
                digitalWrite(_m1Pin, LOW);
                break;
            case MODE_2_POWER_SAVIN:
                digitalWrite(_m0Pin, LOW);
                digitalWrite(_m1Pin, HIGH);
                break;
            case MODE_3_SLEEP:
                digitalWrite(_m0Pin, HIGH);
                digitalWrite(_m1Pin, HIGH);
                break;
            default:
                return;
        }
        WaitAUX_H();
        delay(10);
    }
}

void AS32_Driver::cleanUARTBuf() {
    while (_serial->available()) {
        _serial->read();
    }
}

void AS32_Driver::triple_cmd(SLEEP_MODE_CMD_TYPE Tcmd) {
    uint8_t CMD[3] = {(uint8_t)Tcmd, (uint8_t)Tcmd, (uint8_t)Tcmd};
    _serial->write(CMD, 3);
    delay(50);
}

RET_STATUS AS32_Driver::Module_info(uint8_t* pReadbuf, uint8_t buf_len) {
    RET_STATUS STATUS = RET_SUCCESS;
    uint8_t Readcnt = _serial->available();
    if (Readcnt >= buf_len) {
        for(uint8_t idx = 0; idx < buf_len; idx++) {
            *(pReadbuf+idx) = _serial->read();
        }
    } else {
        STATUS = RET_DATA_SIZE_NOT_MATCH;
        cleanUARTBuf();
    }
    return STATUS;
}

RET_STATUS AS32_Driver::Write_CFG_PDS(struct CFGstruct* pCFG) {
    _serial->write((uint8_t *)pCFG, sizeof(CFGstruct));
    WaitAUX_H();
    delay(100);
    return RET_SUCCESS;
}

RET_STATUS AS32_Driver::Read_CFG(struct CFGstruct* pCFG) {
    cleanUARTBuf();
    triple_cmd(R_CFG);
    return Module_info((uint8_t *)pCFG, sizeof(CFGstruct));
}

RET_STATUS AS32_Driver::Read_module_version(struct MVerstruct* MVer) {
    cleanUARTBuf();
    triple_cmd(R_MODULE_VERSION);
    return Module_info((uint8_t *)MVer, sizeof(MVerstruct));
}

void AS32_Driver::Reset_module() {
    triple_cmd(W_RESET_MODULE);
    WaitAUX_H();
    delay(1000);
}

RET_STATUS AS32_Driver::SleepModeCmd(uint8_t CMD, void* pBuff) {
    RET_STATUS STATUS = RET_SUCCESS;
    WaitAUX_H();
    SwitchMode(MODE_3_SLEEP);
    switch (CMD) {
        case W_CFG_PWR_DWN_SAVE:
            STATUS = Write_CFG_PDS((struct CFGstruct* )pBuff);
            break;
        case R_CFG:
            STATUS = Read_CFG((struct CFGstruct* )pBuff);
            break;
        case R_MODULE_VERSION:
            Read_module_version((struct MVerstruct* )pBuff);
            break;
        case W_RESET_MODULE:
            Reset_module();
            break;
        default:
            return RET_INVALID_PARAM;
    }
    WaitAUX_H();
    return STATUS;
}

RET_STATUS AS32_Driver::sendMessage(String msg) {
    SwitchMode(MODE_0_NORMAL);
    if(ReadAUX() != HIGH) {
        return RET_NOT_IMPLEMENT;
    }
    _serial->print(msg);
    WaitAUX_H();
    return RET_SUCCESS;
}

int AS32_Driver::available() {
    return _serial->available();
}

String AS32_Driver::receiveMessage() {
    SwitchMode(MODE_0_NORMAL);
    String msg = "";
    while (_serial->available()) {
        msg += (char)_serial->read();
        delay(2);
    }
    return msg;
}
