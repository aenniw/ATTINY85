# HID-Emulator

Projects is part of [ESP HID-Emulator](https://github.com/aenniw/ESP8266/tree/master/HID-Emulator) project.

#### Used pin-outs

| Digispark ATTiny85    | Value | Function                      |
|:---------------------:|:-----:|:------------------------------|
| D1                    | 1     | TXD                           |
| D2                    | 2     | RXD                           |
| G                     | -     | Ground                        |
| 5V                    | -     | 5V                            |

#### Keyboard layout
Currently only QWERTY US layout translation is supported by DigiKeyboard library see: [Issue](https://github.com/digistump/DigistumpArduino/issues/46)
which causes misinterpretation of KEYs provided to DigiKeyboard and resolution of HID device. 

#### References
[Digispark SoftSerial library](https://github.com/J-Rios/Digispark_SoftSerial-INT0),
[Debug Tools](https://github.com/digistump/DigisparkExamplePrograms/tree/master/Python/DigiUSB/source)