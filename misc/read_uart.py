#
# Read UART on Raspberry Pi
# On the Raspberry Pi, use those pins:
# - GPIO14 (aka UART0_TXD), pin #8           - green - Not mandatory, we read only.
# - GPIO15 (aka UART0_RXD), pin #10          - white
# - GND, pins #6, 9, 14, 20, 25, 30, 34, 39  - black
# - 5V, pins #2, 4                           - red
#
# run with "sudo python3 read_uart.py"
#
# make sure "enable_uart=1" in /boot/config.txt
#
import time
import serial
               
CR = b'\r'
NL = b'\n'

port='/dev/serial0'
# port='/dev/serial1'
# port='/dev/ttyS0'
# port='/dev/ttyACM0'
# port='/dev/ttyAMA0'

baudrate=9600

print("Reading serial port {}:{}".format(port, baudrate))

ser = serial.Serial(            
     port=port,
     baudrate=baudrate,
     parity=serial.PARITY_NONE,
     stopbits=serial.STOPBITS_ONE,
     bytesize=serial.EIGHTBITS,
     timeout=1
)
time.sleep(1)

cr = False
nl = False

ba = b''
keep_looping = True
while keep_looping:
	try:
		if ser.inWaiting() > 0:
			ser_data = ser.read()
			ba += ser_data
			if cr == False and ser_data == CR:
				cr = True
			if cr == True and ser_data == NL:
				nl = True
			if cr and nl:  # NMEA String completed
				nmea_string = ba.decode("utf-8")
				print("NMEA Data: {}".format(nmea_string[:-2]))  # Drop CR-LF
				# Reset 
				ba = b''
				cr = False
				nl = False
			# print("read from Serial [{}]".format(ser_data))
	except KeyboardInterrupt:
		keep_looping = False
		break     # Theorically useless
	except Exception as ex:
		print("Oops {}".format(ex))


ser.close()
print("\nBye!")
