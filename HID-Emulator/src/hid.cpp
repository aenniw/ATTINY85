#include <Arduino.h>
#include <SoftSerial_INT0.h>
#include <DigiKeyboard.h>

typedef enum {
    K_PRINT, K_DELAY, K_PRESS, K_STROKE
} HID_EVENT_TYPE;

#define P_RX 2                        // Reception PIN (SoftSerial)
#define P_TX 1                        // Transmition PIN (SoftSerial)

SoftSerial serial(P_RX, P_TX);           // Software serial port for control the Bluetooth module

void setup() {
    serial.begin(9600); // Initialize the serial port
}

void loop() {
    DigiKeyboard.update();
    while (serial.available()) {
        char c = (char) serial.read();
        serial.print(c);
    }
}

// l = buffer len | strlen(s) + 1
int parse_int(const char *s, const uint8_t l) {
    uint8_t i;
    // end if c out of range 48 - 57
    for (i = 0; i < l && s[i] >= '0' && s[i] <= '9'; i++);
    int type = -1, exp = 1;
    for (; i > 0; i--) {
        if (exp == 1) type = 0;
        type += (s[i - 1] - 48) * exp;
        exp *= 10;
    }
    return type;
}

// l = buffer len | strlen(s) + 1
int get_next_arg(const char *s, const uint8_t l) {
    int p;
    for (p = 0; p < l && s[p] != ' '; p++);
    return p;
}

void recieve(const char *data, uint8_t len) {
    const HID_EVENT_TYPE type = (HID_EVENT_TYPE) parse_int(data, len + 1);
    const int arg1 = get_next_arg(data, len), arg2 = arg1 + get_next_arg(data + arg1, len - arg1);
    DigiKeyboard.sendKeyStroke(0);
    switch (type) {
        case K_PRINT:
            DigiKeyboard.println(data + arg1);
            break;
        case K_DELAY:
            DigiKeyboard.delay(parse_int(data + arg1, len - arg1));
            break;
        case K_PRESS:
            if (arg1 == arg2) {
                DigiKeyboard.sendKeyPress((byte) parse_int(data + arg1, len - arg1));
            } else {
                DigiKeyboard.sendKeyPress((byte) parse_int(data + arg1, len - arg1),
                                          (byte) parse_int(data + arg2, len - arg2));
            }
            break;
        case K_STROKE:
            if (arg1 == arg2) {
                DigiKeyboard.sendKeyStroke((byte) parse_int(data + arg1, len - arg1));
            } else {
                DigiKeyboard.sendKeyStroke((byte) parse_int(data + arg1, len - arg1),
                                           (byte) parse_int(data + arg2, len - arg2));
            }
            break;
        default:
            break;
    }
}
