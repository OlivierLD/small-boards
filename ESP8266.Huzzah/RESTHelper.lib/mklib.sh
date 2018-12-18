#!/usr/bin/env bash
#
# Archiving RESTHelper
#
echo -e "+----------------------+"
echo -e "| Archiving RESTHelper |"
echo -e "+----------------------+"
zip -r RESTHelper.zip RESTHelper/
#
echo -e "You can now un-archive RESTHelper.zip into your Arduino libraries directory"
echo -e "or use the Sketch > Include Library > Add .ZIP Library... in the Arduino IDE menu"
#
