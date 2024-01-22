# Example Servo Code
# Control the angle of a STANDARD Servo Motor
#
# Intercative UI
#
import RPi.GPIO as GPIO
from time import sleep

ONE_GLOBAL: int      = 1
RESET_EACH_TIME: int = 2

PWM_OPTION: int = RESET_EACH_TIME

SERVO_PIN: int = 11   # aka GPIO_17, physical #11

GPIO.setmode(GPIO.BOARD)
GPIO.setup(SERVO_PIN, GPIO.OUT)

pwm: GPIO.PWM = GPIO.PWM(SERVO_PIN, 50)
# print(f"pwm is a {type(pwm)}")
if PWM_OPTION == ONE_GLOBAL:
    pwm.start(0)


def setAngle(angle: float) -> None:
    if PWM_OPTION == RESET_EACH_TIME:
        pwm.start(0)
    duty: float = (angle / 18) + 2
    GPIO.output(SERVO_PIN, True)
    pwm.ChangeDutyCycle(duty)
    sleep(0.5)  # ??
    GPIO.output(SERVO_PIN, False)
    pwm.ChangeDutyCycle(duty)
    if PWM_OPTION == RESET_EACH_TIME:
        pwm.stop()

print("Initializing servo at 90-deg")
setAngle(90)

keep_working: bool =  True
user_input: str = ""

print("-- Ready. Enter the servo angle when prompted (Q to exit).")
while keep_working:
    user_input = input("Servo Angle [0..180] (Q to exit): ")
    if user_input.upper() == 'Q':
        print("Exiting at user's request")
        keep_working = False
    else:
        try:
            angle: float = float(user_input.strip())
            if angle < 0 or angle > 180:
                print(f"Value between 0 and 180, please. Not {angle}.")
            else:
                print(f"setting servo to {angle}")
                # TODO: a smooth move, step by step ?
                setAngle(angle)            
        except Exception as ex:
            print(f"Error converting {user_input} to a decimal number: {ex}")
            pass

print("Exiting the loop")
print("Parking at 90-deg")
setAngle(90)

if PWM_OPTION == ONE_GLOBAL:
    pwm.stop()
GPIO.cleanup()
print("Bye!")
