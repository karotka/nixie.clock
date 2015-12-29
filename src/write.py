#!/usr/bin/env python

# python startup file
import serial
import readline
import rlcompleter
import atexit
import os
# tab completion
readline.parse_and_bind("bind ^I rl_complete")
# history file
histfile = os.path.join(os.environ['HOME'], '.pythonhistory')
readline.read_history_file(histfile)
atexit.register(readline.write_history_file, histfile)

SERIAL  = "/dev/tty.usbserial-A90ZBP5L"
ser = serial.Serial(SERIAL, 9600, stopbits=2)

while 1:
    line = raw_input().strip()

    if line.startswith("T:"):
        line = line[2:]
        hour, min = line.split(" ")
        ser.write('T')
        ser.write(chr(int(hour)))
        ser.write(chr(int(min)))

    if line.startswith("L:"):
        line = line[2:]
        r, g, b, l = line.split(" ")
        ser.write('L') # G
        ser.write(chr(int(r))) # R
        ser.write(chr(int(g))) # G
        ser.write(chr(int(b))) # B
        ser.write(chr(int(l))) # B

    ser.write("\n")
    print "Value:%s" % line

del os, histfile, readline, rlcompleter
