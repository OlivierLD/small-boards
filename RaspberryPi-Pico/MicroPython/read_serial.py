#
# Read a GPS
#
# Connections/pinout
# Red  : pin #39 (VSYS)
# Black: pin #38 (GND)
# Green: pin #22 (GP17)
# White: pin #21 (GP16)
#
from machine import UART, Pin

TX_PIN=16   # Pin(16) = GP16, pin #21. Green wire - Not required
RX_PIN=17   # Pin(17) = GP17, pin #22. White wire
BAUD_RATE=9600

log_file = open("gps_log.nmea", "w")

uart = UART(0, baudrate=BAUD_RATE, tx=Pin(TX_PIN), rx=Pin(RX_PIN), bits=8, parity=None, stop=1)

keep_looping = True
while keep_looping:
	try:
		# print("Any: {}".format(uart.any()))
	    if uart.any():
	          rcvChar = uart.read(1)  # 1 byte
	          nmea_string = rcvChar.decode("ascii")
	          print(nmea_string)   # , end="")
	          log_file.write(nmea_string)
	          log_file.flush()
	except KeyboardInterrupt:
		keep_looping = False
		break     # Theorically useless
	except Exception as ex:
		print("Oops {}".format(ex))


uart.close()
log_file.close()
