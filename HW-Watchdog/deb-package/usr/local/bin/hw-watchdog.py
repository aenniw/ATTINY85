#!/usr/bin/python

import logging
import time
import usb  # 1.0 not 0.4


def getStringDescriptor(device, index):
    response = device.ctrl_transfer(usb.util.ENDPOINT_IN,
                                    usb.legacy.REQ_GET_DESCRIPTOR,
                                    (usb.util.DESC_TYPE_STRING << 8) | index,
                                    0,  # language id
                                    255)  # length
    # TODO: Refer to 'libusb_get_string_descriptor_ascii' for error handling
    return response[2:].tostring().decode('utf-16')


REQUEST_TYPE_SEND = usb.util.build_request_type(usb.util.CTRL_OUT,
                                                usb.util.CTRL_TYPE_CLASS,
                                                usb.util.CTRL_RECIPIENT_DEVICE)

REQUEST_TYPE_RECEIVE = usb.util.build_request_type(usb.util.CTRL_IN,
                                                   usb.util.CTRL_TYPE_CLASS,
                                                   usb.util.CTRL_RECIPIENT_DEVICE)

USBRQ_HID_GET_REPORT = 0x01
USBRQ_HID_SET_REPORT = 0x09
USB_HID_REPORT_TYPE_FEATURE = 0x03


class ArduinoUsbDevice(object):
    def __init__(self, idVendor, idProduct):
        self.idVendor = idVendor
        self.idProduct = idProduct

        self.device = usb.core.find(idVendor=self.idVendor,
                                    idProduct=self.idProduct)

        if not self.device:
            raise Exception("Device not found")

    def write(self, byte):
        self._transfer(REQUEST_TYPE_SEND, USBRQ_HID_SET_REPORT,
                       byte,
                       [])  # ignored

    def write_string(self, data):
        for c in data:
            try:
                self.write(ord(c))
                time.sleep(send_delay)
            except:
                time.sleep(retry_delay)
                logging.debug('Retry write.')

    def read(self):
        response = self._transfer(REQUEST_TYPE_RECEIVE, USBRQ_HID_GET_REPORT,
                                  0,  # ignored
                                  1)  # length

        if not response:
            raise Exception("No Data")

        return response[0]

    def read_until(self, delim='\0'):
        data = ""
        while True:
            try:
                data += chr(self.read())
                time.sleep(send_delay)
                if data.endswith(delim):
                    break
            except:
                time.sleep(retry_delay)
                logging.debug('Retry read.')
        return data.replace("\n", ", ").upper().strip()

    def _transfer(self, request_type, request, index, value):
        return self.device.ctrl_transfer(request_type, request,
                                         (USB_HID_REPORT_TYPE_FEATURE << 8) | 0,
                                         index,
                                         value)

    @property
    def productName(self):
        return getStringDescriptor(self.device, self.device.iProduct)

    @property
    def manufacturer(self):
        return getStringDescriptor(self.device, self.device.iManufacturer)


keep_alive = 60
send_delay = 0.001
retry_delay = 0.2

logging.basicConfig(format='%(asctime)s\t%(levelname)s\t%(message)s', datefmt='%m/%d/%Y %I:%M:%S %p',
                    level=logging.INFO)

try:
    theDevice = ArduinoUsbDevice(idVendor=0x16c0, idProduct=0x05df)

    theDevice.write_string("STATUS")
    response = theDevice.read_until()
    logging.info(response)
    if not "OK" in response:
        raise Exception("Status check failed.")

    if not "ARMED: TRUE" in response.upper():
        theDevice.write_string("SET " + str(keep_alive * 1000))
        response = theDevice.read_until()
        logging.info(response)
        if not "OK" in response:
            raise Exception("Timeout not set.")

        theDevice.write_string("START")
        response = theDevice.read_until()
        logging.info(response)
        if not "OK" in response:
            raise Exception("Starting failed.")

    while True:
        theDevice.write_string("PING")
        response = theDevice.read_until()
        logging.info(response)
        if "OK" in response:
            time.sleep(keep_alive / 3)
        else:
            time.sleep(retry_delay)
            logging.warning('Retry PING.')
except Exception, e:
    logging.error('Device error: ' + str(e))
