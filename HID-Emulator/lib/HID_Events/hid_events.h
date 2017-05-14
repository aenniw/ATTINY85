#ifndef ATTINY85_PROJECTS_ROOT_HID_EVENTS_H
#define ATTINY85_PROJECTS_ROOT_HID_EVENTS_H

#include <Arduino.h>

typedef enum {
    NONE,
    K_PRINT, K_DELAY, K_PRESS, K_STROKE
} HID_EVENT_TYPE;

typedef struct {
    const char *data;
} HID_K_PRINT;

typedef struct {
    uint32_t len;
} HID_K_DELAY;

typedef struct {
    byte key;
    byte modifiers;
} HID_K_PS;

union HID_EVENT_UNION {
    HID_K_DELAY k_delay;
    HID_K_PS k_ps;
    HID_K_PRINT k_print;
};

typedef struct {
    HID_EVENT_TYPE type;
    union HID_EVENT_UNION event;
} HID_EVENT;

void parse_event(const char *, uint8_t, HID_EVENT *);

#endif //ATTINY85_PROJECTS_ROOT_HID_EVENTS_H
