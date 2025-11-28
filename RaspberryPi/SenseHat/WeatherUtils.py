import math


#
# Compliant with http://www.dpcalc.org/
#
# @param hum in %
# @param temp in Celsius
# @return dew point temperature in Celsius
#
def dew_point_temperature(hum : float, temp: float) -> float:
    dewPointTemp: float = 0
    c1: float = 6.10780
    c2: float = 17.08085 if (temp > 0) else 17.84362
    c3: float = 234.175 if (temp > 0) else 245.425

    pz: float = c1 * math.exp((c2 * temp) / (c3 + temp))
    pd: float = pz * (hum / 100)

    dewPointTemp = (- math.log(pd / c1) * c3) / (math.log(pd / c1) - c2)

    return dewPointTemp


#
# See https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/
# Absolute Humidity (grams/m3) = (6.112 × e^[(17.67 × T)/(T+243.5)] × rh × 2.1674) / (273.15+T)
# @param temp Air Temp in Celsius                                                                                                                                                  * @param rh Relative Humidity in %
# @return the Absolute humidity in g/m3
#
def absolute_humidity(temp: float, rh: float) -> float:
    ah: float = (6.112 * math.exp((17.67 * temp) / (temp + 243.5)) * rh * 2.1674) / (273.15 + temp)
    return ah


if __name__ == '__main__':
    airTemp: float = 20.0
    relHum: float = 65.0
    dewPointTemp = dew_point_temperature(relHum, airTemp)
    rounded = round(dewPointTemp * 10) / 10.0
    print(f"Dew Point Temp: {dewPointTemp:.5f}\272C => Rounded {rounded}\272C")
    # assertEquals("Wrong Value", 13.2, rounded);
    airTemp = 12.9
    relHum = 95.1
    absHum: float = absolute_humidity(airTemp, relHum)
    print(f"For temp {airTemp}\272C, Rel Hum {relHum}%, Abs Hum = {absHum:.2f} g/m3")
