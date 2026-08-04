#ifndef COM_H
#define COM_H

#include <Arduino.h>
#include <LoRa_E32.h>

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
    LoRa_E32 e32;
};

#endif
