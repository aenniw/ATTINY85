#include <Arduino.h>
#include <SoftSerial_INT0.h>

#define __KEYBOARD__

#ifdef __KEYBOARD__
#include <DigiKeyboard.h>
#endif
#ifdef __MOUSE__
#include <DigiMouse.h>
#endif

#define AT_OK "AT->OK"
#define AT_NOK "AT->NOK"
#define AT_NA "AT->NA"
#define AT_B "AT->BUSY"

#define P_RX 2                        // Reception PIN (SoftSerial)
#define P_TX 1                        // Transmition PIN (SoftSerial)
#define BUFFER_MAX 256

typedef enum {
    NOP = 0,
    K_DELAY = 1, K_PRESS = 2, K_STROKE = 3, K_PRINT = 4,
    M_DELAY = 11, M_PRESS = 12, M_STROKE = 13, M_MOVE = 14, M_SCROLL = 15
} AT_TYPE;

SoftSerial serial(P_RX, P_TX);           // Software serial port for control the Bluetooth module

void setup() {
    serial.begin(9600); // Initialize the serial port
}

static int readString(char *s, const char end = '\0') {
    char c, len = 0;
    while (serial.available() && len < BUFFER_MAX) {
        c = (char) serial.read();
        if (c == end) {
            break;
        }
        s[len] = c;
        len++;
    }
    return len;
}

static int parse_uint(const char *msg, uint8_t *offset) {
    for (; *offset < BUFFER_MAX && (msg[*offset] > '9' || msg[*offset] < '0'); (*offset)++);
    const int number = atoi(msg + *offset);
    for (; *offset < BUFFER_MAX && msg[*offset] <= '9' && msg[*offset] >= '0'; (*offset)++);
    return number;
}

const char *handleAT(const char *msg) {
    if (msg[0] == 'A' && msg[1] == 'T' && msg[2] == '+') {
        return AT_NOK;
    }
    uint8_t offset = 3;
    const AT_TYPE type = (const AT_TYPE) parse_uint(msg, &offset);
    const int arg1 = parse_uint(msg, &offset), arg2 = parse_uint(msg, &offset);
    switch (type) {
#ifdef __KEYBOARD__
        case K_PRINT:
            DigiKeyboard.println(msg + offset);
            return AT_OK;
        case K_DELAY:
            if (arg1 <= 0) return AT_NOK;
            DigiKeyboard.delay(arg1);
            return AT_OK;
        case K_PRESS:
            if (arg1 <= 0) return AT_NOK;
            DigiKeyboard.sendKeyPress((byte) arg1, (byte) arg2);
            return AT_OK;
        case K_STROKE:
            if (arg1 <= 0) return AT_NOK;
            DigiKeyboard.sendKeyStroke((byte) arg1, (byte) arg2);
            return AT_OK;
#endif
#ifdef __MOUSE__
        case M_DELAY:
            return AT_OK;
        case M_PRESS:
            return AT_OK;
        case M_STROKE:
            return AT_OK;
        case M_MOVE:
            return AT_OK;
        case M_SCROLL:
            return AT_OK;
#endif
    }
    return AT_NA;
}


void loop() {
    char msg[BUFFER_MAX] = {'\0'};
    if (readString(msg, '\n')) {
        serial.print("Recieved: ");
        serial.println((const char *) msg);
        serial.println(handleAT(msg));
    }
#ifdef __KEYBOARD__
    DigiKeyboard.update();
#endif
#ifdef __MOUSE__
    DigiMouse.update();
#endif
}