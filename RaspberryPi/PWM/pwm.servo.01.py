# Example Servo Code
# Control the angle of a 
# Servo Motor with Raspberry Pi

# free for use without warranty
# www.learnrobotics.org

import RPi.GPIO as GPIO
from time import sleep

SERVO_PIN: int = 11   # aka GPIO_17, physical #11

GPIO.setmode(GPIO.BOARD)
GPIO.setup(SERVO_PIN, GPIO.OUT)

pwm = GPIO.PWM(SERVO_PIN, 50)
print(f"pwm is a {type(pwm)}")
pwm.start(0)

def setAngle(angle):
    duty = angle / 18 + 2
    GPIO.output(SERVO_PIN, True)
    pwm.ChangeDutyCycle(duty)
    sleep(1)
    GPIO.output(SERVO_PIN, False)
    pwm.ChangeDutyCycle(duty)

count: int = 0
numLoops: int = 2

while count < numLoops:
    print("set to 0-deg")
    setAngle(0)
    sleep(1)

    print("set to 90-deg")
    setAngle(90)
    sleep(1)

    print("set to 135-deg")
    setAngle(135)
    sleep(1)
    
    count += 1

print("Parking to 90-deg")
setAngle(90)

pwm.stop()
GPIO.cleanup()
