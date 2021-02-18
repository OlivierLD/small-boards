from machine import UART, Pin

TX_PIN=16   # Pin(16) = GP16, pin #21. White wire
RX_PIN=17   # Pin(17) = GP17, pin #22. Green wire
BAUD_RATE=4800

uart = UART(0, baudrate=BAUD_RATE, tx=Pin(TX_PIN), rx=Pin(RX_PIN), bits=8, parity=None, stop=1)

while True:
	# print("Any: {}".format(uart.any()))
    if uart.any():
          rcvChar = uart.read(1)  # 1 byte
          print(rcvChar.decode("ascii"), end="")

