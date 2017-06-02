#include <Arduino.h>
#include <SoftSerial_INT0.h>
#define __KEYBOARD__
#ifdef __KEYBOARD__
#include <DigiKeyboard.h>
#endif
#ifdef __MOUSE__
#include <DigiMouse.h>
const PROGMEM char usbHidReportDescriptor[] = { /* USB report descriptor */
        0x05, 0x01,                                         // USAGE_PAGE (Generic Desktop)
        0x09, 0x02,                                         // USAGE (Mouse)
        0xa1, 0x01,                                         // COLLECTION (Application)
        0x09, 0x01,                                         //		USAGE_PAGE (Pointer)
        0xa1, 0x00,                                         //		COLLECTION (Physical)
        0x05, 0x09,                                         //		USAGE_PAGE (Button)
        0x19, 0x01,                                         //		USAGE_MINIMUM (Button 1)
        0x29, 0x03,                                         //		USAGE_MAXIMUM (Button 3)
        0x15, 0x00,                                         //		LOGICAL_MINIMUM (0)
        0x25, 0x01,                                         //		LOGICAL_MAXIMUM (1)
        0x95, 0x03,                                         //		REPORT_COUNT (3)
        0x75, 0x01,                                         //		REPORT_SIZE (1)
        0x81, 0x02,                                         //		INPUT (Data,Var,Abs)
        0x95, 0x01,                                         //		REPORT_COUNT (1)
        0x75, 0x05,                                         //		REPORT_SIZE (5)
        0x81, 0x01,                                         //		Input(Cnst)
        0x05, 0x01,                                         //		USAGE_PAGE(Generic Desktop)
        0x09, 0x30,                                         //		USAGE(X)
        0x09, 0x31,                                         //		USAGE(Y)
        0x15, 0x81,                                         //		LOGICAL_MINIMUM (-127)
        0x25, 0x7f,                                         //		LOGICAL_MAXIMUM (127)
        0x75, 0x08,                                         //		REPORT_SIZE (8)
        0x95, 0x02,                                         //		REPORT_COUNT (2)
        0x81, 0x06,                                         //		INPUT (Data,Var,Rel)
        0x09, 0x38,                                            //	 Usage (Wheel)
        0x95, 0x01,                                            //	 Report Count (1),
        0x81, 0x06,                                            //	 Input (Data, Variable, Relative)
        0xc0,                                                        // END_COLLECTION
        0xc0                                                     // END_COLLECTION
};
#endif

#define P_RX 2                        // Reception PIN (SoftSerial)
#define P_TX 1                        // Transition PIN (SoftSerial)

typedef enum {
    NOP = 0,
#ifdef __KEYBOARD__
    K_PRINT = 1, K_DELAY = 2, K_PRESS = 3, K_STROKE = 4
#endif
#ifdef __MOUSE__
    M_MOVE = 11, M_DELAY = 12, M_PRESS = 13, M_CLICK = 14, M_SCROLL = 15
#endif
} AT_TYPE;

SoftSerial serial(P_RX, P_TX);

void setup() {
    serial.begin(9600);
#ifdef __MOUSE__
    DigiMouse.begin();
#endif
}

uint8_t l = 0, t = 0, p_prev = 0;

void loop() {
    if (serial.available()) {
        if (!l) {
            l = (uint8_t) serial.read();
            p_prev = t = NOP;
        } else if (!t) t = (uint8_t) serial.read();
        else {
            const uint8_t p = (uint8_t) serial.read();
#ifdef __KEYBOARD__
            // If not using plentiful DigiKeyboard.delay(), make sure to call
            // DigiKeyboard.update() at least every 50ms
            DigiKeyboard.sendKeyStroke(0);
            if (t == K_PRINT) {
                DigiKeyboard.write(p);
            } else if (t == K_DELAY) {
                DigiKeyboard.delay(p);
            } else if (t == K_PRESS) {
                if (l > 1) p_prev = p;
                else if (p_prev) DigiKeyboard.sendKeyPress(p_prev, p);
                else DigiKeyboard.sendKeyPress(p);
            } else if (t == K_STROKE) {
                if (l > 1) p_prev = p;
                else if (p_prev) DigiKeyboard.sendKeyStroke(p_prev, p);
                else DigiKeyboard.sendKeyStroke(p);
            } else serial.read();
#endif
#ifdef __MOUSE__
            // If not using plentiful DigiMouse.delay(), make sure to call
            // DigiMouse.update() at least every 50ms
            if (t == M_DELAY) {
                DigiMouse.delay(p);
            } else if (t == M_MOVE) {
                if (l > 1) p_prev = p;
                else if (p_prev) DigiMouse.move((char) p_prev, (char) p, 0);;
                else DigiMouse.move((char) p, 0, 0);
            } else if (t == M_SCROLL) {
                DigiMouse.scroll((char) p);
            } else if (t == M_PRESS) {
                DigiMouse.setButtons((unsigned char) p);
            } else if (t == M_CLICK) {
                if (p == 0) DigiMouse.leftClick();
                else if (p == 1) DigiMouse.middleClick();
                else if (p == 2) DigiMouse.rightClick();
            } else serial.read();
#endif
        }
        if (l)l--;
    }
#ifdef __KEYBOARD__
    DigiKeyboard.update();
#endif
#ifdef __MOUSE__
    DigiMouse.update();
#endif
}