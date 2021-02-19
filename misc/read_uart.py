#
# Read UART on Raspberry Pi
# On the Raspberry Pi, use those pins:
# - GPIO14 (aka UART0_TXD), pin #8           - green
# - GPIO15 (aka UART0_RXD), pin #10          - white
# - GND, pins #6, 9, 14, 20, 25, 30, 34, 39  - black
# - 5V, pins #2, 4                           - red
#
import time
import serial
               
ser = serial.Serial(            
     port='/dev/serial0',
     baudrate = 9600,
     parity=serial.PARITY_NONE,
     stopbits=serial.STOPBITS_ONE,
     bytesize=serial.EIGHTBITS,
     timeout=1
)

keep_looping = True
while keep_looping:
	try:
		ser_data = ser.read()
		print("read from Serial [{}]".format(ser_data))
	except KeyboardInterrupt:
		keep_looping = False
		break     # Theorically useless
	except Exception as ex:
		print("Oops {}".format(ex))


ser.close()
print("Bye!")
