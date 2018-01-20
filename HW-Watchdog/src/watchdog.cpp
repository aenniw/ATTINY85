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

unsigned long lastPing = 0, parmDelay = 5, seconds = 1;
const uint8_t buffer_size = 16;
char command[buffer_size] = {'\0'};
int pos = 0, lastRead = -1;
boolean at_parsed = false;
long delayCycles = 0;

void setup() {
    pinMode(RESET_PIN, OUTPUT);
    digitalWrite(RESET_PIN, HIGH);
    STREAM.begin();
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}


void loop() {
    if (STREAM.available()) {
        lastRead = STREAM.read();
        if (lastRead == '\0') {
            at_parsed = true;
        }
        command[pos] = (char) lastRead;
        pos = (pos + 1) % buffer_size;
    }
    if (at_parsed) {
        boolean valid_command = true;
        if (strcmp(command, "NOP") == 0);
        else if (lastPing > 0 && strcmp(command, "PING") == 0) {
            lastPing = seconds;
        } else if (lastPing == 0 && pos > 4 && strncmp(command, "SET ", 4) == 0) {
            parmDelay = 0;
            for (int i = pos - 2; i > 3; i--) {
                parmDelay += (command[i] - '0') * (int) (pow(10, pos - 2 - i));
            }
        } else if (lastPing == 0 && strcmp(command, "START") == 0) {
            lastPing = seconds;
        } else if (strcmp(command, "STATUS") == 0) {
            STREAM.write("ARMED: ");
            STREAM.write(lastPing ? "True" : "False");
            STREAM.write('\n');
        } else if (lastPing > 0 && strcmp(command, "STOP") == 0) {
            lastPing = 0;
        } else {
            valid_command = false;
        }

        if (valid_command) {
            STREAM.write("OK:");
        } else {
            STREAM.write("Unknown cmd: ");
        }
        STREAM.write(command);
        STREAM.write((byte) '\0');
        pos = at_parsed = false;
    }

    STREAM.refresh();
    // Seconds tick approximation
    if (delayCycles == 0) {
        const auto led_state = uint8_t(digitalRead(LED_PIN)),
                reset_state = uint8_t(digitalRead(RESET_PIN));
        digitalWrite(LED_PIN, !led_state);

        if (lastPing > 0 && seconds > lastPing + parmDelay) {
            if (!reset_state) {
                lastPing = 0;
            }
            digitalWrite(RESET_PIN, !reset_state);
        }

        delayCycles = long(F_CPU * 0.003);
        seconds++;
    }
    delayCycles--;
}