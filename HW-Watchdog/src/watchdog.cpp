#include <Arduino.h>
#include <DigiUSB.h>

#define STREAM DigiUSB

#define LED_PIN     15
#define RESET_PIN   17
#define DEF_DELAY   60
#define TO_SECONDS  1000

unsigned long startTime = 0, lastPing = 0, parmDelay = DEF_DELAY * TO_SECONDS;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(RESET_PIN, OUTPUT);
    STREAM.begin();
}

static char read_char() {
    char lastRead = '\0';
    if (STREAM.available()) {
        lastRead = static_cast<char>(STREAM.read());
        STREAM.delay(10);
    }
    return lastRead;
}

static String read_line() {
    String cmd;
    char c = read_char();
    while (c != '\n' && c != '\0') {
        cmd += c;
        c = read_char();
    }
    return cmd;
}

static void blink_led(const uint8_t pin, const unsigned long delay) {
    digitalWrite(pin, HIGH);
    STREAM.delay(delay);
    digitalWrite(pin, LOW);
}

static void trigger_reset(const uint8_t pin, const unsigned long delay) {
    digitalWrite(pin, HIGH);
    STREAM.delay(delay);
    digitalWrite(pin, LOW);
}

static void print_status() {
    STREAM.println(F(" START                start the watchdog"));
    STREAM.println(F(" STATUS               display the current watchdog status"));
    STREAM.println(F(" STOP                 stop the watchdog"));
}

static void set_options(const String &command) {
    if (command.startsWith("DELAY ", 4) && command.length() > 10) {
        String rest = command.substring(10);
        long long_delay = rest.toInt();
        if (long_delay >= 1 && long_delay <= 65535) {
            parmDelay = (unsigned long) long_delay * TO_SECONDS;
        }
    }
}

void loop() {
    const String command = read_line();
    if (command.length() > 0) {
        boolean valid_command = true;
        if (command.equals("PING")) {
            blink_led(LED_PIN, 100);
            lastPing = millis();
        } else if (command.startsWith("REBOOT ")) {
            lastPing = 0;
        } else if (command.startsWith("SET ")) {
            set_options(command);
        } else if (command.equals("START")) {
            startTime = millis();
        } else if (command.equals("STATUS")) {
            print_status();
        } else if (command.equals("STOP")) {
            startTime = 0;
        } else {
            valid_command = false;
        }

        if (valid_command) {
            STREAM.print(F("OK: "));
        } else {
            STREAM.print(F("Unknown or invalid command: "));
        }
        STREAM.println(command);
    }

    if (startTime > 0 && millis() > lastPing + parmDelay) {
        trigger_reset(RESET_PIN, 100);
        startTime = 0;
    }
    STREAM.refresh();
}