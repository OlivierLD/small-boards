#!/bin/bash
# nohup python3 SenseHatRESTServer.py --machine-name:$(hostname -I) --port:9999 --verbose:false &
python3 SenseHatRESTServer.py --machine-name:$(hostname -I) --port:9999 --verbose:false &
echo $! > SenseHatRESTServer.pid
echo "SenseHatRESTServer started with PID $(cat SenseHatRESTServer.pid)"
echo "Try curl -X GET http://$(hostname -I | awk '{print $1}'):9999/sense-hat/oplist | jq"


