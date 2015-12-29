#!/usr/bin/env python

import serial

SERIAL  = "/dev/tty.usbserial-A90ZBP5L"

ser = serial.Serial(SERIAL, 9600, stopbits=2)


def readData():
    buff = []
    while 1:
        v = ser.read()
        if ord(v) == 10:
            print "". join(buff)
            return
        else:
            buff.append(v)

def readData1():
    print ord(ser.read())

while 1:
    readData()
