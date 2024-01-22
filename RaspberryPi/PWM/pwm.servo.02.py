# Example Servo Code
# Control the angle of a STANDARD Servo Motor
#
# Intercative UI
#
import RPi.GPIO as GPIO
from time import sleep


SERVO_PIN: int = 11   # aka GPIO_17, physical #11
SMOOTH_MOVE: bool = False

GPIO.setmode(GPIO.BOARD)
GPIO.setup(SERVO_PIN, GPIO.OUT)

pwm: GPIO.PWM = GPIO.PWM(SERVO_PIN, 50)
# print(f"pwm is a {type(pwm)}")
pwm.start(0)


def setAngle(angle: float) -> None:
    duty: float = (angle / 18) + 2
    GPIO.output(SERVO_PIN, True)
    pwm.ChangeDutyCycle(duty)
    sleep(0.5)  # ??
    GPIO.output(SERVO_PIN, False)
    pwm.ChangeDutyCycle(duty)

servo_current_pos: int = 90
print("Initializing servo at 90-deg")
setAngle(servo_current_pos)

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
                if SMOOTH_MOVE:
                    incr: int = 1
                    if servo_current_pos > angle:
                        incr = -1
                    for step in range(servo_current_pos + incr, angle, incr):
                        setAngle(step)
                        sleep(0.5)
                else:
                    setAngle(angle)            
                servo_current_pos = angle
        except Exception as ex:
            print(f"Error converting {user_input} to a decimal number: {ex}")
            pass

print("Exiting the loop")
print("Parking at 90-deg")
setAngle(90)

pwm.stop()
GPIO.cleanup()
print("Bye!")
