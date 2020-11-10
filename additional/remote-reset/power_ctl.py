#!/usr/bin/python3
import RPi.GPIO as GPIO
from time import sleep
import sys

RESET_PIN = 3
POWER_PIN = 2

class power_ctl:
    def __init__(self):
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
        GPIO.setup(RESET_PIN, GPIO.OUT)
        GPIO.setup(POWER_PIN, GPIO.OUT)
        GPIO.output(RESET_PIN, GPIO.HIGH)   # RESET is high by default
        GPIO.output(POWER_PIN, GPIO.HIGH)

    def restart(self, delay=1):
        GPIO.output(RESET_PIN, GPIO.LOW)
        sleep(int(delay))
        GPIO.output(RESET_PIN, GPIO.HIGH)

    def powerbutton(self, delay=3):
        GPIO.output(POWER_PIN, GPIO.LOW)
        sleep(int(delay))
        GPIO.output(POWER_PIN, GPIO.HIGH)

if (__name__== "__main__"):
    if(len(sys.argv) == 1):
        print ("Power button tool")
        print ("<power/restart> delay")
        sys.exit(0)
    
    p = power_ctl();

    if (sys.argv[1] == "power"):
        if (len(sys.argv) == 2):
            print("power button signal sent")
            p.powerbutton()
        elif(len(sys.argv) == 3):
            print("power button signal sent")
            p.powerbutton(sys.argv[2])
    elif (sys.argv[1] == "restart"):
        if (len(sys.argv) == 2):
            print("restart button signal sent")
            p.restart()
        elif(len(sys.argv) == 3):
            print("restart button signal sent")
            p.restart(sys.argv[2])


