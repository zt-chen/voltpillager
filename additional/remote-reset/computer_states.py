import pexpect
import serial
from pexpect_serial import SerialSpawn
import os
from power_ctl import *
import time

hostname="tryagain-Z170X-Gaming-3"

def is_loggedin(ss):
    try:
        ss.sendline('')
        ss.expect(hostname+':~$',2)
        loggedin = True
    except:
        loggedin = False

    return loggedin

def try_login(ss,username,password):
    for i in range(0,2):
        try:
            if ( is_loggedin(ss) ):
                return True 

            ss.sendline('')
            ss.expect('login', 2 )
            ss.sendline(username)
            ss.expect('Password', 2 )
            ss.sendline(password)

            if ( is_loggedin(ss) ):
                return True 
        except:
            pass

    return False


c_pwr = power_ctl()
prev_state = "OK"
with serial.Serial('/dev/ttyS0', 9600, timeout=0) as ser:
    while(1):
        ss = SerialSpawn(ser)

        if ( not try_login(ss, 'tryagain', 'tryagain')):
            # Either its stuck or not powered on

            print("Computer is probably mad and it doesn't speak to me anymore")
            # Most probably it's a freeze since it was ok before 
            if ( prev_state == "OK" ):
                print("Lets try to calm it down with a restart")
                c_pwr.restart()
                prev_state = "RESTART"
            else:
                print("Even restart didn't fix it, now it's electricity time")
                c_pwr.powerbutton()
                prev_state = "POWER"

            time.sleep(50)

        else:
            print("Computer is healthy and happy")
            prev_state = "OK"
            time.sleep(10)
            

