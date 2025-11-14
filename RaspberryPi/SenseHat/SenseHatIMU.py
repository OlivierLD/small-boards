#!/usr/bin/python

# IMU (inertial measurement unit)

from sense_hat import SenseHat

sense = SenseHat()
sense.set_imu_config(True, True, True)  # compass, gyroscope, accelerometer

orientation = sense.get_orientation_degrees()
print("Orientation - p: {pitch}, r: {roll}, y: {yaw}".format(**orientation))

north = sense.get_compass()
print("Compass - North: %s" % north)

# alternatives
print("Alt - 1, North: %s" % sense.compass)

gyro_only = sense.get_gyroscope()
print("Gyro - p: {pitch}, r: {roll}, y: {yaw}".format(**gyro_only))

# alternatives
print("Alt-1 - Gyro: %s" % sense.gyro)
print("Alt-2 - Gyro: %s" % sense.gyroscope)


accel_only = sense.get_accelerometer()
print("Acc - p: {pitch}, r: {roll}, y: {yaw}".format(**accel_only))

# alternatives
print("Alt-1 - Acc: %s" % sense.accel)
print("Alt-2 - Acc: %s" % sense.accelerometer)
