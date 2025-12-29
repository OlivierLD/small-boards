#!/bin/bash
if [ $1 = "--live" ]; then
  python3 SenseHatRESTServer.py --machine-name:$(hostname -I) --port:9999 --verbose:true
else
  nohup python3 SenseHatRESTServer.py --machine-name:$(hostname -I) --port:9999 --verbose:false &
fi
echo $! > SenseHatRESTServer.pid
echo "SenseHatRESTServer started with PID $(cat SenseHatRESTServer.pid)"
echo "Try curl -X GET http://$(hostname -I | awk '{print $1}'):9999/sense-hat/oplist | jq"
