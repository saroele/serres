# -*- coding: utf-8 -*-
"""
Maak eens een koelkast van die diepvriezer.
Hang node N06 in de diepvriezer, en plug de jeelink op de pi.
Zet ook de energenie module op de py en sluit de diepvriezer aan
op stopcontact "1" van energenie.

Start dit python script, en start ook de flask website
koelvriezer_flask.py in folder 'website'

Created on 30/04/2016

@author: roel
"""

from ontvanger import get_single_reading
import json
from energenie import switch_on, switch_off
from time import sleep


def control(tmin, tmax, t, running):
    """

    :param tmin: minimum temp, in °C
    :param tmax: max temp, °C
    :param t: current temp
    :param running: bool: True means freezer is runnin currently
    :return: running: new status for running

    """
    if (t < tmax and t>tmin and running) or t > tmax:
        running = True
    elif t < tmin:
        running = False

    return running


def send_control(control):

    if control:
        for i in range(3):
            switch_on()
            sleep(1)
    else:
        for i in range(3):
            switch_off()
            sleep(1)

if __name__ == '__main__':
    running = False
    while True:

        # read config
        config = json.load(open('koelvriezer.cfg', 'r'))
        # get measurements
        d = get_single_reading()
        # compute control
        print(u"Current temperature: {}°C".format(d['T']))
        running = control(config['tmin'], config['tmax'], float(d['T']), running)
        # send control
        send_control(running)

        status = 'ON' if running else 'OFF'
        print("Control signal {} sent".format(status))

