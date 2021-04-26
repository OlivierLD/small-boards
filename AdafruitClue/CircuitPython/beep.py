from adafruit_clue import clue
import time

clue_display = clue.simple_text_display(text_scale=3, colors=(clue.WHITE,))

clue_display[0].text = "Play a Sound"

while True:
    print("Loop...")
    clue.start_tone(2000)
    time.sleep(1)
    clue.stop_tone()
    time.sleep(2)
