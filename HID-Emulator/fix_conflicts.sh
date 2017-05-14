#!/usr/bin/env bash

exec 1<>/dev/null
exec 2>&1

cd ~/.platformio/packages/framework-arduinoavr/libraries/__cores__
mv ./digispark/DigisparkKeyboard/usbdrvasm.asm ./digispark/DigisparkKeyboard/usbdrvasm.asm_

exit 0