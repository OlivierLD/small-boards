#!/bin/bash
pid=$(ps -ef | grep SenseHatRESTServer.py | grep -v grep | awk '{print $2}')
if [ -z "$pid" ]; then
  echo "SenseHatRESTServer is not running."
else
  kill $pid
  echo "SenseHatRESTServer with PID $pid has been stopped."
fi
