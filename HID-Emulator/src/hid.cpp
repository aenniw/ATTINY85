#include <Arduino.h>
#include <SoftSerial_INT0.h>

#define __KEYBOARD__
#define __DEBUG__

#ifdef __KEYBOARD__
#include <DigiKeyboard.h>
#endif
#ifdef __MOUSE__
#include <DigiMouse.h>
#endif

#define AT_OK "AT->OK"
#define AT_NOK "AT->NOK"
#define AT_NA "AT->NA"
#define AT_EOB "AT->EOB"

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

static int parse_uint(const char *msg, uint8_t *offset) {
    for (; *offset < BUFFER_MAX && (msg[*offset] > '9' || msg[*offset] < '0'); (*offset)++);
    const int number = atoi(msg + *offset);
    for (; *offset < BUFFER_MAX && msg[*offset] <= '9' && msg[*offset] >= '0'; (*offset)++);
    return number;
}

static const char *handleAT(const char *msg) {
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

static char buffer[BUFFER_MAX] = {'\0'} , buffer_pos = 0;
static bool buffer_ready = false;

void loop() {
    if(buffer_ready) {
#ifdef __DEBUG__
        serial.print("Recieved: ");
        serial.println((const char*) buffer);
#else
        handleAT(buffer);
#endif
        buffer_ready = false;
        buffer_pos = 0;
    } else if (serial.available()){
        if(buffer_pos >= BUFFER_MAX){
            serial.println((const char*)AT_EOB);
            buffer_ready = true;
        } else {
            char c = (char) serial.read();
            if (c == '\n') {
                buffer_ready = true;
                buffer[buffer_pos] = '\0';
            } else {
                buffer[buffer_pos++] = c;
            }
        }
    }
#ifdef __KEYBOARD__
    DigiKeyboard.update();
#endif
#ifdef __MOUSE__
    DigiMouse.update();
#endif
}
