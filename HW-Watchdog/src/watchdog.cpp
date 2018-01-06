#include <Arduino.h>
#include <DigiUSB.h>

#define STREAM DigiUSB

#define D0          0
#define D1          1
#define D2          2
#define D3          3
#define D4          4
#define D5          5

#define LED_PIN         D1
#define RESET_PIN       D0
#define TOGGLE_DELAY    1000

unsigned long startTime = 0, lastPing = 0, parmDelay = 60000;

void setup() {
    pinMode(RESET_PIN, OUTPUT);
    digitalWrite(RESET_PIN, HIGH);
    STREAM.begin();
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}

static void toggle(const uint8_t pin, unsigned long delay_time) {
    digitalWrite(pin, LOW);
    STREAM.delay(delay_time);
    digitalWrite(pin, HIGH);
}

const uint8_t buffer_size = 16;

void loop() {
    char command[buffer_size] = {'\0'};
    int pos = 0, lastRead;
    while (pos + 1 < buffer_size) {
        if (STREAM.available()) {
            lastRead = STREAM.read();
            if (lastRead == '\0') {
                break;
            }
            command[pos++] = (char) lastRead;
        }
        STREAM.delay(10);
    }

    if (pos > 0) {
        boolean valid_command = true;
        if (startTime > 0 && strcmp(command, "PING") == 0) {
            toggle(LED_PIN, TOGGLE_DELAY);
            lastPing = millis();
        } else if (startTime == 0 && pos > 4 && strncmp(command, "SET ", 4) == 0) {
            parmDelay = 1;
            for (int i = 1; pos - i > 3; i++) {
                parmDelay += pow(10, i - 1) * (command[pos - i] - 48);
                STREAM.delay(10);
            }
        } else if (startTime == 0 && strcmp(command, "START") == 0) {
            startTime = millis();
        } else if (strcmp(command, "STATUS") == 0) {
            STREAM.write("ARMED: ");
            STREAM.write(startTime ? "True" : "False");
            STREAM.write('\n');
        } else if (startTime > 0 && strcmp(command, "STOP") == 0) {
            startTime = 0;
        } else {
            valid_command = false;
        }

        if (valid_command) {
            STREAM.write("OK:");
        } else {
            STREAM.write("Unknown or invalid command: ");
        }
        STREAM.write(command);
        STREAM.write((byte) '\0');
    }

    if (startTime > 0 && millis() > lastPing + parmDelay) {
        STREAM.delay(10);
        toggle(RESET_PIN, TOGGLE_DELAY);
        startTime = 0;
    }
    STREAM.refresh();
}