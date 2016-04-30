# -*- coding: utf-8 -*-
"""
Created on Sat Jan  4 22:23:02 2014

@author: roel
"""

def parse_answer(s):
    """
    Parse the answer from the serial port, return dictionary with:
    - N: node (int)
    - T: temperature (°C)
    - RH: relative humidity (%)
    - D: dewpoint (°C)
    - B: boiler (0/1)
    - P: pump (0/1)
    """
    
    d = {}
    for sub in s.split():
        if sub.find(':') > 0:
            splitted = sub.split(':')
            d[splitted[0]] = splitted[1]
    return d
            
    
    


import serial

ser = serial.Serial('/dev/ttyUSB0', 57600)
while True:
    # First, send a byte to get things running
    ser.write('b')    
    # now, get node ID, temp and rel hum
    answer = ser.readline()
    print answer
    print parse_answer(answer)
    
