#
# Magnetometer to heading, pitch, roll in degrees 
#
import math

degree_sign= u'\N{DEGREE SIGN}'

mag_x =  15.0
mag_y = -79.6
mag_z = 224.5

heading = math.degrees(math.atan2(mag_y, mag_x))
while heading < 0:
    heading += 360
pitch =  math.degrees(math.atan2(mag_y, mag_z))
roll =  math.degrees(math.atan2(mag_x, mag_z))

# print("Heading: {:.2f}{}".format(heading, degree_sign))
print(f"Heading: {heading:.2f}{degree_sign}")
print(f"Pitch  : {pitch:.2f}{degree_sign}")
print(f"Roll   : {roll:.2f}{degree_sign}")
