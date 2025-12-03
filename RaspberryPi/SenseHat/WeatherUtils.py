import math


#
# Compliant with http://www.dpcalc.org/
#
# @param hum (relative) in %
# @param temp in Celsius
# @return dew point temperature in Celsius
#
def dew_point_temperature(hum : float, temp: float) -> float:
    c1: float = 6.10780
    c2: float = 17.08085 if (temp > 0) else 17.84362
    c3: float = 234.175 if (temp > 0) else 245.425

    pz: float = c1 * math.exp((c2 * temp) / (c3 + temp))
    pd: float = pz * (hum / 100)

    dew_point_temp: float = (- math.log(pd / c1) * c3) / (math.log(pd / c1) - c2)
    return dew_point_temp


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
    air_temp: float = 20.0
    rel_hum: float = 65.0
    dew_point_temp: float = dew_point_temperature(rel_hum, air_temp)
    rounded = round(dew_point_temp * 10) / 10.0
    print(f"Dew Point Temp: {dew_point_temp:.5f}\272C => Rounded {rounded}\272C")
    # assertEquals("Wrong Value", 13.2, rounded);
    air_temp = 12.9
    rel_hum = 95.1
    absHum: float = absolute_humidity(air_temp, rel_hum)
    print(f"For temp {air_temp}\272C, Rel Hum {rel_hum}%, Abs Hum = {absHum:.2f} g/m3")
