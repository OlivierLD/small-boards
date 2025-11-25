#!/bin/bash
nohup python3 SenseHatRESTServer.py --machine-name:$(hostname -I) --port:9999 --verbose:false &
echo $! > SenseHatRESTServer.pid
echo "SenseHatRESTServer started with PID $(cat SenseHatRESTServer.pid)"


