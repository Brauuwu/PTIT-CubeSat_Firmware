#ifndef COM_H
#define COM_H

#include <Arduino.h>

#if USE_AS32_MODULE
#include "AS32_Driver.h"
#else
#include <LoRa_E32.h>
#endif

class PTIT_COM {
public:
    PTIT_COM();

    void init();
    String update();
    void sendPing();
    void sendMessage(String msg);
    
    // API thay đổi tham số vật lý
    bool setChannel(uint8_t channel);
    bool setAddress(uint8_t addh, uint8_t addl);
    bool setTransmissionMode(uint8_t mode);

private:
#if USE_AS32_MODULE
    AS32_Driver as32;
#else
    LoRa_E32 e32;
#endif
};

#endif
