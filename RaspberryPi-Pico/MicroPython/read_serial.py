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

led = Pin(25, Pin.OUT)  # The led


CR = b'\r'
NL = b'\n'

TX_PIN=16   # Pin(16) = GP16, pin #21. Green wire - Not required
RX_PIN=17   # Pin(17) = GP17, pin #22. White wire
BAUD_RATE=9600

log_file = open("gps_log.nmea", "w")

uart = UART(0, baudrate=BAUD_RATE, tx=Pin(TX_PIN), rx=Pin(RX_PIN), bits=8, parity=None, stop=1)

cr = False
nl = False

ba = b''
keep_looping = True
while keep_looping:
	try:
		# print("Any: {}".format(uart.any()))
	    if uart.any():
	        	ser_data = uart.read(1)  # 1 byte
				ba += ser_data
				if cr == False and ser_data == CR:
					cr = True
				if cr == True and ser_data == NL:
					nl = True
				if cr and nl:  # NMEA String completed
					nmea_string = ba.decode("utf-8")
					print("NMEA Data: {}".format(nmea_string[:-2]))  # Drop CR-NL
		            log_file.write(nmea_string)
		            log_file.flush()
					# Reset 
					ba = b''
					cr = False
					nl = False
					# Blink led, to acknowledge
					led.toggle()

	except KeyboardInterrupt:
		keep_looping = False
		break     # Theorically useless
	except Exception as ex:
		print("Oops {}".format(ex))


uart.close()
log_file.close()
