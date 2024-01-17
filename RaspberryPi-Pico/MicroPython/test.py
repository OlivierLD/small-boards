# test.py

def extract_date(nmea_sentence):
    try:
        # Get the current date-time from RMC sentence
        ddmmyy = nmea_sentence[:-3].split(',')[9]
        utc = nmea_sentence[:-3].split(',')[1]
        now = "{}-{}-{} {}:{}:{} UTC".format(ddmmyy[0:2], ddmmyy[2:4], ddmmyy[4:6], utc[0:2], utc[2:4], utc[4:6])
        return now
    except Exception as ex:
        print(ex)
        raise

start_rmc = "$GPRMC,014132.000,A,3745.0115,N,12230.4638,W,1.54,280.74,220221,,,A*7A"
end_rmc =   "$GPRMC,071145.000,A,3744.9360,N,12230.4213,W,0.39,20.56,220221,,,D*4B"

print("From {} to {}".format(extract_date(start_rmc), extract_date(end_rmc)))

print("Done")
