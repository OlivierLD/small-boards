'''
Control the Brightness of LED using PWM on Raspberry Pi
http://www.electronicwings.com

See https://www.electronicwings.com/raspberry-pi/raspberry-pi-pwm-generation-using-python-and-c
Resistor 220 or 230 Ohms on the wiring schema

'''

import RPi.GPIO as GPIO
from time import sleep

ledpin = 12				               # PWM pin connected to LED
GPIO.setwarnings(False)			       # disable warnings
GPIO.setmode(GPIO.BOARD)		       # set pin numbering system
GPIO.setup(ledpin, GPIO.OUT)
pi_pwm = GPIO.PWM(ledpin, 1000)		   # create PWM instance with frequency
pi_pwm.start(0)				           # start PWM of required Duty Cycle 
keep_looping: bool = True

try:
    while keep_looping:
        # print("Increasing...")
        for duty in range(0, 101, 1):
            pi_pwm.ChangeDutyCycle(duty)   # provide duty cycle in the range 0-100
            sleep(0.01)
        sleep(0.5)
        
        # print("Decreasing...")
        for duty in range(100, -1, -1):
            pi_pwm.ChangeDutyCycle(duty)
            sleep(0.01)
        sleep(0.5)
except KeyboardInterrupt:
    keep_looping = False
    print("\n\t\tOver and out!")
    pass

print("Done with PWM.")
