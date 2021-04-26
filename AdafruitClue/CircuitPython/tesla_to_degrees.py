from adafruit_clue import clue
import math
#
# Magnetic Data (Telsa) to Headin, Pitch and Roll in degrees
#
while True:
    mag_x, mag_y, mag_z = clue.magnetic
    heading = math.degrees(math.atan2(mag_y, mag_x))
    while heading < 0:
        heading += 360
    pitch =  math.degrees(math.atan2(mag_y, mag_z))
    roll =  math.degrees(math.atan2(mag_x, mag_z))

    print(f"HDG {heading:.2f}, PCH {pitch:.2f}, ROL {roll:.2f}")
