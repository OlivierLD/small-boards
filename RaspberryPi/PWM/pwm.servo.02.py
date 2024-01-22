# Example Servo Code
# Control the angle of a Servo Motor with Raspberry Pi
#
# Intercative UI
#
import RPi.GPIO as GPIO
from time import sleep

SERVO_PIN: int = 11   # aka GPIO_17, physical #11

GPIO.setmode(GPIO.BOARD)
GPIO.setup(SERVO_PIN, GPIO.OUT)

pwm: GPIO.PWM = GPIO.PWM(SERVO_PIN, 50)
# print(f"pwm is a {type(pwm)}")
pwm.start(0)


def setAngle(angle: float) -> None:
    duty: float = (angle / 18) + 2
    GPIO.output(SERVO_PIN, True)
    pwm.ChangeDutyCycle(duty)
    sleep(1)
    GPIO.output(SERVO_PIN, False)
    pwm.ChangeDutyCycle(duty)

print("Initializing servo at 90-deg")
setAngle(90)

keep_working: bool =  True
user_input: str = ""

while keep_working:
    user_input = input("Servo Angle [0..359] (Q to exit): ")
    if user_input.upper() == 'Q':
        print("Exiting at user's request")
        keep_working = False
    else:
        try:
            angle: float = float(user_input.strip())
            if angle < 0 or angle > 359:
                print(f"Value between 0 and 359, please. Not {angle}.")
            else:
                print(f"setting servo to {angle}")
                # TODO: a smooth move ?
                setAngle(angle)            
        except Exception as ex:
            print(f"Error converting {user_input} to a decimal number: {ex}")
            pass

print("Exiting the loop")
print("Parking at 90-deg")
setAngle(90)

pwm.stop()
GPIO.cleanup()
print("Bye!")
