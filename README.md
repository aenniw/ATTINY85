# ATTINY85 based projects
[![Build Status](https://github.com/aenniw/ATTINY85/workflows/Platformio%20build/badge.svg)](https://github.com/aenniw/ATTINY85/actions?query=workflow%3A%22Platformio+build%22+branch%3Amaster)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=J96CRYPYAY2VE&currency_code=EUR&source=url)

![Digispark ATtiny85 layout](img/Digispark_ATTINY85_layout.jpg)

| Digispark ATTiny85    | Value | Function                      |
|:---------------------:|:-----:|:------------------------------|
| D0                    | 0     | IO, PWM0 AREF, MOSI, SDA      |
| D1                    | 1     | IO, PWM1, MISO                |
| D2                    | 2     | IO, A1, SCK, SCL              |
| D3                    | 3     | IO, A3 USB+                   |
| D4                    | 4     | IO, PWM4, A2, USB-            |
| D5                    | 5     | IO, A0                        |
| G                     | -     | Ground                        |
| 5V                    | -     | 5V                            |
| VIN                   | -     |                               |

## Setup environment
```
sudo pip install platformio

IDE=eclipse ./configure     # for eclipse layout setup
IDE=clion ./configure       # for clion layout setup
```

## Build
```
./make
```

## SubProjects
[HID-Emulator](HID-Emulator/README.md) |
[HW-Watchdog](HW-Watchdog/README.md)