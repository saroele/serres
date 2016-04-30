# -*- coding: utf-8 -*-
"""
Created on Sat Jan  4 22:23:02 2014

@author: roel
"""
import serial
ser = serial.Serial('/dev/ttyUSB0', 57600)

def parse_answer(s):
    """
    Parse the answer from the serial port, return dictionary with:
    - N: node (int)
    - T: temperature (°C)
    - V: relative humidity (%)
    - Boiler: boiler (0/1)
    - Pump: pump (0/1)
    """
    
    d = {}
    for sub in s.split():
        if sub.startswith('N'):
            d['N'] = sub[1:]
        elif sub.find('=') > 0:
            splitted = sub.split('=')
            d[splitted[0]] = splitted[1]
    return d
            
    
def get_single_reading():
    """
    Return dictionary with single reading for N06

    """

    while True:
        # First, send a byte to get things running
        ser.write('b')
        # now, get node ID, temp and rel hum
        answer = ser.readline()
        d = parse_answer(answer)
        try:
            if d['N'] == '06':
                return d
        except:
            pass
