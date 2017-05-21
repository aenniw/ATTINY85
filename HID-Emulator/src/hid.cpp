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


typedef enum {
    NOP = 0,
    K_DELAY = 1, K_PRESS = 2, K_STROKE = 3, K_PRINT = 4,
    M_DELAY = 11, M_PRESS = 12, M_STROKE = 13, M_MOVE = 14, M_SCROLL = 15
} AT_TYPE;

SoftSerial serial(P_RX, P_TX);           // Software serial port for control the Bluetooth module

void setup() {
    serial.begin(9600); // Initialize the serial port
}

int readString(String &s, const char end = '\0') {
    char c, len = 0;
    while (serial.available()) {
        c = (char) serial.read();
        if (c == end) {
            break;
        }
        s += c;
        len++;
    }
    return len;
}

int parse_uint(const String &msg) {
    const int msg_len = msg.length();
    int i;
    for (i = msg.indexOf('+') + 1; i < msg_len; i++) {
        msg.charAt((unsigned int) i);
    }

    return -1;
}


const char *handleAT(const String &msg) {
    if (!msg.startsWith("AT+")) {
        return AT_NOK;
    }
    const AT_TYPE type = (const AT_TYPE) parse_uint(msg);
    const int p_start = msg.indexOf(' ');
    if (p_start <= 0) return AT_NOK;
    const String payload = msg.substring((unsigned int) p_start + 1).trim();
    int arg1 = -1, arg2 = -1;
    switch (type) {
#ifdef __KEYBOARD__
        case K_PRINT:
            //DigiKeyboard.println(payload);
            return AT_OK;
        case K_DELAY:
            arg1 = parse_uint(payload);
            if (arg1 < 0) return AT_NOK;
            //DigiKeyboard.delay(arg1);
            return AT_OK;
        case K_PRESS:
            arg1 = parse_uint(payload),
                    arg2 = parse_uint(payload.substring((unsigned int) (payload.indexOf(' ') + 1)));
            if (arg1 < 0) return AT_NOK;
            if (arg2 < 0) DigiKeyboard.sendKeyPress((byte) arg1);
            else DigiKeyboard.sendKeyPress((byte) arg1, (byte) arg2);
            return AT_OK;
        case K_STROKE:
            arg1 = parse_uint(payload),
                    arg2 = parse_uint(payload.substring((unsigned int) (payload.indexOf(' ') + 1)));
            if (arg1 < 0) return AT_NOK;
            if (arg2 < 0) DigiKeyboard.sendKeyStroke((byte) arg1);
            else DigiKeyboard.sendKeyStroke((byte) arg1, (byte) arg2);
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
    String msg;
    if (readString(msg, '\n')) {
        serial.print("Recieved: ");
        serial.println(msg);
        serial.println(handleAT(msg.trim()));
    }
#ifdef __KEYBOARD__
    DigiKeyboard.update();
#endif
#ifdef __MOUSE__
    DigiMouse.update();
#endif
}

//// UI ------------------------------------------------------------------------
// > [MODE=03/K_PRINT] [ARGS...]
// > K_STROKE T L_CTRL+L_SHIFT
// > echo "Hello World"   <-->   K_PRINT echo "Hello World"

// Web UI -> ws->stream cmds to esp -> stream ATs via i2c to ATtiny -> execute AT
// Web UI <- ws->stream resp from esp <- stream ATs via i2c from ATtiny <- AT response
// Web UI -> Status / Administration / HID / Logout
// Web UI -> HID -> cmd log, cmd input, send cmd


//// KEYBOARD ------------------------------------------------------------------

// AT+01 -> STROKE
// AT+01 [KEY] [?MODIFICATOR]
// AT+01 (uint) (uint)
// AT+01 parse_int() ?parse_int()

// AT+02 -> PRESS
// AT+02 [KEY] [?MODIFICATOR]
// AT+02 (uint) (uint)
// AT+02 parse_int() ?parse_int()

// AT+03 -> DELAY
// AT+03 [TIME]
// AT+03 (uint)
// AT+03 parse_int()

// AT+04 -> PRINT
// AT+04 [MSG]
// AT+04 (char*)
// AT+04 reuse_rest_of_buffer()


//// MOUSE ---------------------------------------------------------------------

// AT+11 -> STROKE
// AT+11 [KEY]
// AT+00 (uint)
// AT+11 parse_int()

// AT+12 -> PRESS
// AT+12 [KEY]
// AT+00 (uint)
// AT+12 parse_int()

// AT+13 -> DELAY
// AT+13 [TIME]
// AT+00 (uint)
// AT+13 parse_int()

// AT+14 -> MOVE
// AT+14 [X] [Y]
// AT+00 (int) (int)
// AT+14 parse_int() parse_int()

// AT+15 -> SCROLL
// AT+15 [S]
// AT+00 (int)
// AT+15 parse_int()