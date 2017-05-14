#include <Arduino.h>
#include <TinyWireS.h>
#define __DEBUG__
#ifndef __DEBUG__
#include <DigiKeyboard.h>
#else
#include <DigiUSB.h>
#endif

#include "hid_events.h"

#define I2C_ADDRESS   0x48              // 7 bit I2C address

void execute_event(const HID_EVENT event) {
#ifndef __DEBUG__
    #define EVENT_DELAY 500
    DigiKeyboard.sendKeyStroke(0);
    switch (event.type) {
        case K_PRINT:
            DigiKeyboard.println(event.event.k_print.data);
            DigiKeyboard.delay(EVENT_DELAY);
            break;
        case K_DELAY:
            DigiKeyboard.delay(event.event.k_delay.len);
            break;
        case K_PRESS:
            DigiKeyboard.sendKeyPress(event.event.k_ps.key, event.event.k_ps.modifiers);
            DigiKeyboard.delay(EVENT_DELAY);
            break;
        case K_STROKE:
            DigiKeyboard.sendKeyStroke(event.event.k_ps.key, event.event.k_ps.modifiers);
            DigiKeyboard.delay(EVENT_DELAY);
            break;
    }
#else

#endif
}

void i2c_recieve(uint8_t len) {
    char data[len] = {'\0'};
    for (uint32_t i = 0; i < len && TinyWireS.available(); i++) {
        data[i] = (char) TinyWireS.receive();
    }
    HID_EVENT e = {.type = NONE, .event = {}};
    parse_event(data, len, &e);
    execute_event(e);
#ifdef __DEBUG__
    DigiUSB.write(data);
#endif
}

void setup() {
    TinyWireS.begin(I2C_ADDRESS);
    TinyWireS.onReceive(i2c_recieve);
#ifdef __DEBUG__
    DigiUSB.begin();
#endif
}

void loop() {
#ifndef __DEBUG__
    DigiKeyboard.update();
#else
    DigiUSB.println("Waiting for input...");
    DigiUSB.delay(1000);
#endif
}
