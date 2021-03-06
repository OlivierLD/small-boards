#
# Read a GPS
#
# Connections/pinout
# Red  : pin #39 (VSYS)
# Black: pin #38 (GND)
# Green: pin #22 (GP17)
# White: pin #21 (GP16)
#
"""
Read a GPS - Connected as explained
Log GPS Data into a file
"""
from machine import UART, Pin

led = Pin(25, Pin.OUT)  # The led

TX_PIN=16   # Pin(16) = GP16, pin #21. Green wire - Not required
RX_PIN=17   # Pin(17) = GP17, pin #22. White wire
BAUD_RATE=9600

FILE_PATH = "gps_log.nmea"

log_file = open(FILE_PATH, "w")

uart = UART(0, baudrate=BAUD_RATE, tx=Pin(TX_PIN), rx=Pin(RX_PIN), bits=8, parity=None, stop=1)

nb_rec = 0;
log_size = 0;
keep_looping = True
while keep_looping:
    try:
        # print("Any: {}".format(uart.any()))
        if uart.any():
                nmea_string = uart.readline().decode("utf-8")
                print("NMEA Data: {}".format(nmea_string[:-2])) 
                if (nmea_string.startswith("$GPRMC")):  # Filter
                    nb_char_written = log_file.write(nmea_string)  # Write it with the CR-NL
                    log_file.flush()
                    log_size += nb_char_written
                    nb_rec += 1
                    print("\t>> {} record(s) in the log ({:,} bytes)".format(nb_rec, log_size)) 
                # Blink led, to acknowledge
                led.toggle()

    except KeyboardInterrupt:
        keep_looping = False
        print("Exiting at user's request")
        led.value(0)   # Switch led off
        break     # Theorically useless
    except OSError as oserr:
        if str(oserr) == "28":   # errno.ENOSPC:
            now = None
            if (nmea_string.startswith("$GPRMC")):
                try:
                    # Get the current date-time from RMC sentence
                    ddmmyy = nmea_string[:-3].split(',')[9]
                    utc = nmea_string[:-3].split(',')[1]
                    now = "{}-{}-{} {}:{}:{} UTC".format(ddmmyy[0:2], ddmmyy[2:4], ddmmyy[4:6], utc[0:2], utc[2:4], utc[4:6])
                except Exception as ex:
                    pass
            print("Drive is full, exiting{}.".format((" at " + now) if now != None else ""))
            led.value(0)   # Switch led off
            keep_looping = False
        else:
            print("OSError ?? [{}]".format(str(oserr)))
    except Exception as ex:
        print("Exception: {}".format(ex.__class__.__name__))
        print("[{}]".format(str(ex)))
        print(ex)

# uart.close()
log_file.close()
