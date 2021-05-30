from adafruit_clue import clue
import math
#
# Magnetic Data (Telsa) to Heading, Pitch and Roll in degrees
#
clue_display = clue.simple_text_display(text_scale=3, colors=(clue.CYAN,))

clue_display[0].text = "Tesla to"
clue_display[1].text = "     Degrees"
clue_display[2].text = "--------------"
clue_display[2].color = clue.RED

clue_display[3].color = clue.WHITE
clue_display[4].color = clue.GREEN
clue_display[5].color = clue.ORANGE

terminal = False

while True:
    mag_x, mag_y, mag_z = clue.magnetic
    heading = math.degrees(math.atan2(mag_y, mag_x))
    while heading < 0:
        heading += 360
    pitch = math.degrees(math.atan2(mag_y, mag_z))
    roll = math.degrees(math.atan2(mag_x, mag_z))

    if terminal:
        print(f"HDG {heading:.2f}, PCH {pitch:.2f}, ROL {roll:.2f}")
    else:
        clue_display[3].text = f"HDG   {heading:6.2f}"
        clue_display[4].text = f"PITCH {pitch:6.2f}"
        clue_display[5].text = f"ROLL  {roll:6.2f}"
        # 
        clue_display.show()

