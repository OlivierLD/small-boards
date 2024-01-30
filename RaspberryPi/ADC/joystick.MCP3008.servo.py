#
# Read pot values from a joystick, through the MCP3008
# Neutral pos is U/D: 0.5, L/R: 0.5
#
# Drive a pan-tilt stand.
#
# Neutral position for both servos: 90 deg, value 0.5
#

import gpiozero 
from gpiozero import MCP3008
from time import sleep
import RPi.GPIO as GPIO

#  GPIO.setmode(GPIO.BOARD) # Not compatible with MCP3008 ?
GPIO.setmode(GPIO.BCM)
SERVO_UP_DOWN_PIN: int    = 17   # aka GPIO_17, physical #11
SERVO_LEFT_RIGHT_PIN: int = 18   # aka GPIO_18, physical #12

GPIO.setup(SERVO_UP_DOWN_PIN, GPIO.OUT)
GPIO.setup(SERVO_LEFT_RIGHT_PIN, GPIO.OUT)

pwm_up_down: GPIO.PWM = GPIO.PWM(SERVO_UP_DOWN_PIN, 50)
pwm_left_right: GPIO.PWM = GPIO.PWM(SERVO_LEFT_RIGHT_PIN, 50)

pwm_up_down.start(0)
pwm_left_right.start(0)

print("Servos are initialized")

def setAngleUpDown(angle: float) -> None:
    duty: float = (angle / 18) + 2
    GPIO.output(SERVO_UP_DOWN_PIN, True)
    pwm_up_down.ChangeDutyCycle(duty)
    sleep(0.5)  # ??
    GPIO.output(SERVO_UP_DOWN_PIN, False)
    pwm_up_down.ChangeDutyCycle(duty)

def setAngleLeftRight(angle: float) -> None:
    duty: float = (angle / 18) + 2
    GPIO.output(SERVO_LEFT_RIGHT_PIN, True)
    pwm_left_right.ChangeDutyCycle(duty)
    sleep(0.5)  # ??
    GPIO.output(SERVO_LEFT_RIGHT_PIN, False)
    pwm_left_right.ChangeDutyCycle(duty)

# [0..1] to [0..180]
def adcValueToAngle(value: float) -> int:
    return round(value * 180)

print("Initializing servos at 90-deg")
servo_current_ud_pos: int = 90
servo_current_lr_pos: int = 90
setAngleUpDown(servo_current_ud_pos)
setAngleLeftRight(servo_current_lr_pos)


MCP3008_CHANNEL_0: int = 0  # in [0..7]
MCP3008_CHANNEL_1: int = 1  # in [0..7]
# create an object per pot. MCP3008 channel 0, and 1
pot_up_down: gpiozero.spi_devices.MCP3008    = MCP3008(MCP3008_CHANNEL_0)
pot_left_right: gpiozero.spi_devices.MCP3008 = MCP3008(MCP3008_CHANNEL_1)
# print(f"Pot is a {type(pot)}")

prev_value_up_down: float    = -10
prev_value_left_right: float = -10

# Notice: pot values go from 0 to 1, and NOT from 0 to 1024 !!
try:
    while True:
        if (abs(prev_value_up_down - pot_up_down.value) > 0.001):        # value has changed
            print(f"Up-Down    value: {pot_up_down.value}")
            setAngleUpDown(adcValueToAngle(pot_up_down.value))
        if (abs(prev_value_left_right - pot_left_right.value) > 0.001):  # value has changed
            print(f"Left-Right value: {pot_left_right.value}")
            setAngleLeftRight(adcValueToAngle(pot_left_right.value))

        prev_value_up_down    = pot_up_down.value
        prev_value_left_right = pot_left_right.value

        sleep(0.1)
except KeyboardInterrupt:
    print("\nBye now!")
    pass

print("Parking servos")
setAngleUpDown(90)
setAngleLeftRight(90)

pwm_up_down.stop()
pwm_left_right.stop()
GPIO.cleanup()

print("Done with MCP3008 and joystick")
