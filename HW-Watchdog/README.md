# HW-Watchdog

### Parts
*   [DigiStump](https://www.ebay.com/itm/Digispark-Kickstarter-ATTINY85-Arduino-General-Micro-USB-Development-Board/221889695650?epid=1669310069&hash=item33a9a80fa2:g:GdoAAOSwiLdV~BaV) or other solutions based on ATTINY85
*   [Relay](https://www.ebay.com/itm/1PCS-5V-One-1-Channel-Relay-Module-Board-Shield-For-PIC-AVR-ARM-Arduino/301924608045?epid=1066441792&hash=item464c1be82d:g:DSoAAOSw5VFWKdoC) Single channel relay

### Optional Parts
*   [Case - TODO](models/)

#### Used pin-outs

| Digispark ATTiny85    | Value | Function                      |
|:---------------------:|:-----:|:------------------------------|
| D0                    | 0     | Relay                           |
| G                     | -     | Ground                        |
| 5V                    | -     | 5V                            |

#### Dependencies

```pip install pyusb argparse```

#### Usage

    - starting hw-watchdog will automaticaly arm watchdog
    - to disarm watchdog  any instances of hw-watchdog needs to be terminated and startend with param -d
    - watchdog wimeout can be set in seconds, with default value 60s
    - once trigered watchdog trigers relay for 1s to simulate reset button press