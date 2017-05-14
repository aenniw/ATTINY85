#include <Arduino.h>
#include "DigiKeyboard.h"

typedef enum {
    K_PRINT, K_DELAY, K_PRESS, K_STROKE
} HID_EVENT_TYPE;

typedef struct {
    char *data;
} HID_K_PRINT;

typedef struct {
    uint32_t len;
} HID_K_DELAY;

typedef struct {
    byte press;
    byte modifiers;
} HID_K_PRESS;

typedef struct {
    byte stroke;
    byte modifiers;
} HID_K_STROKE;

union HID_EVENT_UNION {
    HID_K_DELAY k_delay;
    HID_K_PRESS k_press;
    HID_K_PRINT k_print;
    HID_K_STROKE k_stroke;
};

typedef struct {
    HID_EVENT_TYPE type;
    union HID_EVENT_UNION event;
} HID_EVENT;


void execute_event(const HID_EVENT event) {
    switch (event.type) {
        case K_PRINT:
            DigiKeyboard.println(event.event.k_print.data);
            break;
        case K_DELAY:
            DigiKeyboard.delay(event.event.k_delay.len);
            break;
        case K_PRESS:
            DigiKeyboard.sendKeyPress(event.event.k_press.press, event.event.k_press.modifiers);
            break;
        case K_STROKE:
            DigiKeyboard.sendKeyStroke(event.event.k_stroke.stroke, event.event.k_stroke.modifiers);
            break;
    }
}

const HID_EVENT events[] = {
        {.type = K_PRESS, .event = {
                .k_press = {
                        .press= KEY_T,
                        .modifiers = MOD_CONTROL_LEFT | MOD_ALT_LEFT
                }}},
        {.type = K_PRINT, .event = {
                .k_print = {
                        .data = (char *) "echo \"Hello world\""
                }}},
        {.type = K_PRESS, .event = {
                .k_press = {
                        .press= KEY_ENTER,
                        .modifiers = 0
                }}},
        {.type = K_DELAY, .event = {
                .k_delay = {
                        .len = 5000
                }}},
        {.type = K_PRESS, .event = {
                .k_press = {
                        .press= KEY_D,
                        .modifiers = MOD_CONTROL_LEFT
                }}},
        {.type = K_DELAY, .event = {
                .k_delay = {
                        .len = 5000
                }}}
};
const size_t events_len = sizeof(events) / sizeof(HID_EVENT);

void setup() {
    // this is generally not necessary but with some older systems it seems to
    // prevent missing the first character after a delay:
    DigiKeyboard.sendKeyStroke(0);
}

void loop() {
    for (uint32_t i = 0; i < events_len; i++) {
        execute_event(events[i]);
        DigiKeyboard.sendKeyStroke(0);
    }
}
