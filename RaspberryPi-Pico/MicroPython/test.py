#
# See what exception is raised when the drive is full
#
nmea_string = '$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W,A*6A'
now = None
if (nmea_string.startswith("$GPRMC")):
	try:
		ddmmyy = nmea_string[:-3].split(',')[9]
		utc = nmea_string[:-3].split(',')[1]
		now = "{}-{}-{} {}:{}:{} UTC".format(ddmmyy[0:2], ddmmyy[2:4], ddmmyy[4:6], utc[0:2], utc[2:4], utc[4:6])
	except Exception as ex:
		pass
print("Drive is full, exiting{}.".format((" at " + now) if now != None else ""))
