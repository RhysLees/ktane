// CAN.h
#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

extern "C"
{
#include "can2040.h"
}

typedef struct can2040 CANHandle;
typedef struct can2040_msg CANMsg;

class CAN
{
public:
    CAN(uint8_t rxPin, uint8_t txPin, uint16_t rxId);

    void setup();
    void transmit(uint16_t id, uint8_t data);

private:
    static CAN *instancePointer;
    static CANHandle handle;
    uint32_t sys_clock = 125000000;
    uint32_t bitrate = 125000;
    uint8_t rxPin;
    uint8_t txPin;
    uint16_t rxId;

    static void canCallback(CANHandle *cd, uint32_t notify, CANMsg *msg);
    static void pioIrqHandler();
};
