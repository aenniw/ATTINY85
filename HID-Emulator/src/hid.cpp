#include <Arduino.h>
#include <DigiKeyboard.h>

void setup() {

}

size_t write(uint8_t chr) {

    uint8_t key = pgm_read_word_near(chr);
    uint8_t mod = pgm_read_word_near(chr + 1);
    DigiKeyboard.sendKeyStroke(key, mod);
    //uint8_t data = pgm_read_byte_near(ascii_to_scan_code_table + (chr - 8));
    //DigiKeyboard.sendKeyStroke(data & 0b11111111, data >> 7 ? MOD_SHIFT_RIGHT : 0);
    //DigiKeyboard.sendKeyStroke(data & 0b11111111, data >> 7 ? MOD_SHIFT_RIGHT : 0);
    return 1;
}

void print(const char *c) {
    while (c++ != '\0') {
        write((uint8_t) *c);
    }
}

void loop() {
    print("abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890 !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");
    DigiKeyboard.delay(10000);
    DigiKeyboard.update();
}