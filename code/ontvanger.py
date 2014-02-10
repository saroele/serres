# -*- coding: utf-8 -*-
"""
Created on Sat Jan  4 22:23:02 2014

@author: roel
"""

import serial
ser = serial.Serial('/dev/ttyUSB0', 57600)
while True:
    print ser.readline()
