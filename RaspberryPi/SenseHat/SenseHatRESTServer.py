#!/usr/bin/env python3
#
# An example (WiP), showing how to have a REST and Web server.
# A Skeleton for further dev.
#
# Requires:
# ---------
# pip3 install http (already in python3.7+, no need to install it)
#
# Provides REST access to the JSON data, try GET http://localhost:8080/sense-hat/oplist
# Can act as a sensor reader.
#
# Start it with
# $ python3 <...>/SenseHatRESTServer.py --machine-name:$(hostname -I) --port:9999 --verbose:false
#
# TODO Fix verbose pb. See in the code. See "self.path.startswith(PATH_PREFIX + "/verbose"):"
#
import json
import signal
import sys
import os
# import traceback
import time
import random
# import math
import threading
from http.server import HTTPServer, BaseHTTPRequestHandler
from typing import Dict
from datetime import datetime, timezone
# import NMEABuilder   # local script
# import utils         # local script

# sys.path.append('/usr/lib/python3/dist-packages/sense_hat')
from sense_hat import SenseHat
import WeatherUtils

__version__ = "0.0.1"
__repo__ = "https://github.com/OlivierLD/small-boards"

PATH_PREFIX: str = "/sense-hat"
STATIC_PATH_PREFIX: str = "/web"  # Whatever starts with /web is managed as static resource
# TODO zip prefix ? That'd be kewl...
server_port: int = 8080
verbose: bool = False
machine_name: str = "127.0.0.1"

MACHINE_NAME_PRM_PREFIX: str = "--machine-name:"
PORT_PRM_PREFIX: str = "--port:"
VERBOSE_PREFIX: str = "--verbose:"

keep_looping: bool = True
between_loops: int = 1

server_pid: int = os.getpid()  # Used to kill the process. Bam.

sample_data: Dict[str, str] = {  # Used for VIEW (and non-implemented) operations. Fallback.
    "1": "First",
    "2": "Second",
    "3": "Third",
    "4": "Fourth"
}

sense = SenseHat()


# Defining an HTTP request Handler class
class ServiceHandler(BaseHTTPRequestHandler):
    # verbose: bool = False

    # sets basic headers for the server
    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        # reads the length of the Headers
        length = int(self.headers['Content-Length'])
        # reads the contents of the request
        content = self.rfile.read(length)
        temp = str(content).strip('b\'')
        self.end_headers()
        return temp

    # To silence the HTTP logger
    @staticmethod
    def log_message(fmt, *args):
        if verbose:
            print(fmt % args)
        return

    # GET Method Definition
    def do_GET(self):
        if verbose:
            print("GET methods")  # That one works. Same in do_POST does not.
        #
        full_path = self.path
        split = full_path.split('?')
        path = split[0]
        qs = None
        if len(split) > 1:
            qs = split[1]
        # The parameters into a map
        prm_map = {}
        if qs is not None:
            qs_prms = qs.split('&')
            for qs_prm in qs_prms:
                nv_pair = qs_prm.split('=')
                if len(nv_pair) == 2:
                    prm_map[nv_pair[0]] = nv_pair[1]
                else:
                    print("oops, no equal sign in {}".format(qs_prm))

        if path == PATH_PREFIX + "/data":  # Random data, with dates
            if verbose:
                print("JSON Array Value request")
            try:
                json_data: str = json.dumps(DATA_ARRAY)
                # defining all the headers
                self.send_response(200)
                # self.send_header('Content-Type', 'text/plain')
                self.send_header('Content-Type', 'application/json')
                content_len = len(json_data)
                self.send_header('Content-Length', str(content_len))
                self.end_headers()
                self.wfile.write(json_data.encode())
            except Exception as exception:
                error = {"message": "{}".format(exception)}
                self.wfile.write(json.dumps(error).encode())
                self.send_response(500)
        elif path == PATH_PREFIX + "/oplist":  # Warning ! That one is NOT dynamic, like the Java version...
            response = {
                "oplist": [{
                    "path": PATH_PREFIX + "/oplist",
                    "verb": "GET",
                    "description": "Get the available operation list."
                }, {
                    "path": PATH_PREFIX + "/exit",
                    "verb": "POST",
                    "description": "Careful: terminate the server process."
                }, {
                    "path": PATH_PREFIX + "/verbose",
                    "verb": "POST",
                    "description": "Set verbose to true or false, passed in the request's payload."
                }, {
                    "path": PATH_PREFIX + "/honk",
                    "verb": "POST",
                    "description": "Display a warning. Payload can be 'Warning!', '!', or anything."
                }, {
                    "path": PATH_PREFIX + "/temperature",
                    "verb": "GET",
                    "description": "Get the Temperature from the Sense-HAT, in JSON format."
                }, {
                    "path": PATH_PREFIX + "/pressure",
                    "verb": "GET",
                    "description": "Get the Pressure from the Sense-HAT, in JSON format."
                }, {
                    "path": PATH_PREFIX + "/rel-humidity",
                    "verb": "GET",
                    "description": "Get the Relative Humidity from the Sense-HAT, in JSON format."
                }, {
                    "path": PATH_PREFIX + "/all-env-sensors",
                    "verb": "GET",
                    "description": "Get the Temperature, Pressure, and Relative Humidity from the Sense-HAT, in JSON format."
                }, {
                    "path": PATH_PREFIX + "/all-imu-sensors",
                    "verb": "GET",
                    "description": "Get heading, pitch, roll, and yaw, in JSON format."
                }]
            }
            response_content = json.dumps(response).encode()
            self.send_response(200)
            # defining the response headers
            self.send_header('Content-Type', 'application/json')
            content_len = len(response_content)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(response_content)
        elif path == PATH_PREFIX + "/temperature":
            response = {"temperature": sense.get_temperature()}
            response_content = json.dumps(response).encode()
            self.send_response(200)
            # defining the response headers
            self.send_header('Content-Type', 'application/json')
            content_len = len(response_content)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(response_content)
        elif path == PATH_PREFIX + "/pressure":
            response = {"pressure": sense.get_pressure()} # / 3.8954914563 ?
            response_content = json.dumps(response).encode()
            self.send_response(200)
            # defining the response headers
            self.send_header('Content-Type', 'application/json')
            content_len = len(response_content)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(response_content)
        elif path == PATH_PREFIX + "/rel-humidity":
            response = {"rel-humidity": sense.get_humidity()}
            response_content = json.dumps(response).encode()
            self.send_response(200)
            # defining the response headers
            self.send_header('Content-Type', 'application/json')
            content_len = len(response_content)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(response_content)
        elif path == PATH_PREFIX + "/all-env-sensors":
            rel_hum: float = sense.get_humidity()
            air_temp: float = sense.get_temperature()
            response = {
                "rel-humidity": rel_hum,
                "pressure": sense.get_pressure(),      # divide by 3.8954914563 ?
                "temperature": air_temp,
                "dew-point": WeatherUtils.dew_point_temperature(rel_hum, air_temp),
                "abs_hum": WeatherUtils.absolute_humidity(air_temp, rel_hum)
            }
            response_content = json.dumps(response).encode()
            self.send_response(200)
            # defining the response headers
            self.send_header('Content-Type', 'application/json')
            content_len = len(response_content)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(response_content)
        elif path == PATH_PREFIX + "/all-imu-sensors":
            # orientation = sense.get_orientation_degrees()
            # print("Orientation - p: {pitch}, r: {roll}, y: {yaw}".format(**orientation))

            # north = sense.get_compass()
            # print("Compass - North: %s" % north)
            # alternatives
            # print("Alt - 1, North: %s" % sense.compass)

            gyro_only = sense.get_gyroscope()
            if verbose:
                print("Gyro - p: {pitch}, r: {roll}, y: {yaw}".format(**gyro_only))
            # alternatives
            # print("Alt-1 - Gyro: %s" % sense.gyro)
            # print("Alt-2 - Gyro: %s" % sense.gyroscope)

            # accel_only = sense.get_accelerometer()
            # print("Acc - p: {pitch}, r: {roll}, y: {yaw}".format(**accel_only))
            response = {
                "hdg": sense.get_compass(),
                "gyro": {
                    "pitch": gyro_only["pitch"],
                    "roll": gyro_only["roll"],
                    "yaw": gyro_only["yaw"]
                }
            }
            response_content = json.dumps(response).encode()
            self.send_response(200)
            # defining the response headers
            self.send_header('Content-Type', 'application/json')
            content_len = len(response_content)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(response_content)
        elif path.startswith(STATIC_PATH_PREFIX):  # Static content... Content to be fetched in the STATIC_PATH_PREFIX folder (web/ here).
            if verbose:
                print(f"Static path: {path}")
            static_resource: str = path[len(STATIC_PATH_PREFIX):]
            if verbose:
                print(f"Loading static resource [{static_resource}]")

            content_type: str = "text/html"
            binary: bool = False
            if static_resource.endswith(".css"):
                content_type = "text/css"
            elif static_resource.endswith(".js"):
                content_type = "text/javascript"
            elif static_resource.endswith(".png"):
                content_type = "image/png"
                binary = True
            elif static_resource.endswith(".jpg") or static_resource.endswith(".jpeg"):
                content_type = "image/jpg"
                binary = True
            elif static_resource.endswith(".ico"):
                content_type = "image/ico"
                binary = True
            # TODO more cases. jpg, gif, svg, ttf, pdf, wav, etc.
            else:
                print(f"Unmanaged type {static_resource}")

            # Content type based on file extension
            if not binary:
                with open("web" + static_resource) as f:
                    content = f.read()
            else:
                with open("web" + static_resource, "rb") as image:
                    content = image.read()

            if verbose:
                print(f"Data type: {type(content)}, content:\n{content}")
            self.send_response(200)
            # defining the response headers
            self.send_header('Content-Type', content_type)
            content_len = len(content)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            if not binary:
                self.wfile.write(content.encode())
            else:
                self.wfile.write(content)
        else:  # Not found
            if verbose:
                print("GET on {} not managed".format(self.path))
            error = "NOT FOUND! GET on {} not managed.\n".format(self.path)
            self.send_response(400)
            self.send_header('Content-Type', 'text/plain')
            content_len = len(error)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(bytes(error, 'utf-8'))

    # POST method definition
    def do_POST(self):
        if verbose:                                      # Not recognized... wierd.
            print("POST request, {}".format(self.path))
        #
        if self.path.startswith(PATH_PREFIX + "/exit"):
            print(">>>>> REST server received POST /exit")
            # content_len: int = int(self.headers.get('Content-Length'))
            # post_body = self.rfile.read(content_len).decode('utf-8')
            # if verbose:
            #    print("Content: {}".format(post_body))
            response = {"status": "OK"}
            response_content = json.dumps(response).encode()
            self.send_response(201)
            self.send_header('Content-Type', 'application/json')
            content_len = len(response_content)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(response_content)
            time.sleep(2)  # Wait for response to be received
            print(f">>> Killing REST server process ({server_pid}).")
            os.kill(server_pid, signal.SIGKILL)
        if self.path.startswith(PATH_PREFIX + "/honk"):
            content_len: int = int(self.headers.get('Content-Length'))
            post_body = self.rfile.read(content_len).decode('utf-8')
            print(f"Post Body [{post_body}]")  # Debug
            if False:
                sense.set_rotation(90)
                red = (255, 0, 0)
                sense.show_message("Warning!", text_colour=red)
                sense.clear()
            if True:
                sense.set_rotation(90)
                sense.clear()
                if post_body == 'Warning!':
                    sense.load_image("emojis/10.png")  # Warning sign...
                elif post_body == '!':
                    sense.show_message("!!", text_colour=(255, 0, 0))
                else:
                    sense.show_message(post_body, text_colour=(255, 0, 0))
                time.sleep(2)
                sense.clear()
            # REST response stuff
            response = {
                "status": "OK",
                "payload": post_body
            }
            response_content = json.dumps(response).encode()
            self.send_response(201)
            self.send_header('Content-Type', 'application/json')
            content_len = len(response_content)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(response_content)
        elif self.path.startswith(PATH_PREFIX + "/verbose"):
            content_len: int = int(self.headers.get('Content-Length'))
            post_body = self.rfile.read(content_len).decode('utf-8')
            try:
                if verbose:
                    print("Content: {}".format(post_body))
            except Exception as exception:
                error = {"error-message": "{}".format(exception)}
                print(error)
            if post_body.lower() == "true":
                verbose = True
            else:
                verbose = False
            response = {"status": "OK", "verbose": verbose}
            response_content = json.dumps(response).encode()
            self.send_response(201)
            self.send_header('Content-Type', 'application/json')
            content_len = len(response_content)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(response_content)
        else:
            if verbose:
                print("POST on {} not managed".format(self.path))
            error = "NOT FOUND!"
            self.send_response(404)
            self.send_header('Content-Type', 'text/plain')
            content_len = len(error)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(bytes(error, 'utf-8'))

    # PUT method Definition
    def do_PUT(self):
        if verbose:
            print("PUT request, {}".format(self.path))
            print("PUT on {} not managed".format(self.path))
        error = "NOT FOUND!"
        self.send_response(404)
        self.send_header('Content-Type', 'text/plain')
        content_len = len(error)
        self.send_header('Content-Length', str(content_len))
        self.end_headers()
        self.wfile.write(bytes(error, 'utf-8'))

    # DELETE method definition
    def do_DELETE(self):
        if verbose:
            print("DELETE on {} not managed".format(self.path))
        error = "NOT FOUND!"
        self.send_response(400)
        self.send_header('Content-Type', 'text/plain')
        content_len = len(error)
        self.send_header('Content-Length', str(content_len))
        self.end_headers()
        self.wfile.write(bytes(error, 'utf-8'))

    # VIEW method definition. Uncommon...
    def do_VIEW(self):
        if verbose:
            print("VIEW methods")  # That on works. Same in do_POST does not.
        #
        # dict var. for pretty print
        display = {}
        temp = self._set_headers()
        # check if the key is present in the sample_data dictionary
        if temp in sample_data:
            display[temp] = sample_data[temp]
            # print the keys required from the json file
            self.wfile.write(json.dumps(display).encode())
        else:
            error = "{} Not found in sample_data\n".format(temp)
            self.send_response(404)
            self.send_header('Content-Type', 'text/plain')
            content_len = len(error)
            self.send_header('Content-Length', str(content_len))
            self.end_headers()
            self.wfile.write(bytes(error, 'utf-8'))

DATA_ARRAY: Dict[str, int] = {
}
MAP_MAX_LENGTH: int = 10


# THE data producer. To be customized...
def ping_data(dummy_prm: str) -> None:
    global keep_looping
    global between_loops
    global verbose
    global DATA_ARRAY
    global MAP_MAX_LENGTH

    print(f"Data thread")
    while keep_looping:
        # Do you job here
        # print("Pinging data...")
        # Generating data
        utc_ms = datetime.now(timezone.utc).timestamp() * 1_000  # System "UTC epoch" in ms
        dt_object = datetime.fromtimestamp(utc_ms / 1_000, tz=timezone.utc)  # <- Aha !!
        # Duration: YYYY-MM-DDTHH:MI:SS.sss
        # duration_date_time: str = dt_object.strftime("%H%M%S.00,%d,%m,%Y")
        duration_date_time: str = dt_object.strftime("%Y-%m-%dT%H:%M:%S")
        dummy_data: float = random.randrange(-100, 400) / 10
        if verbose:
            print(f"New element {{ 'key':{duration_date_time}, 'value':{dummy_data} }}")
        data: Dict = {}
        data[duration_date_time] = dummy_data
        DATA_ARRAY.update(data)
        # Trim if too long
        while len(DATA_ARRAY) > MAP_MAX_LENGTH:
            key: str = list(DATA_ARRAY.keys())[0]
            if verbose:
                print(f"Dropping {key}, {DATA_ARRAY.get(key)}")
            DATA_ARRAY.pop(key)

        if verbose:
            print(f"\tSleeping between loops for {between_loops} sec.")
        time.sleep(between_loops)  # Wait between loops
    print("\tDone with data thread")


if len(sys.argv) > 0:  # Script name + X args
    for arg in sys.argv:
        if arg[:len(MACHINE_NAME_PRM_PREFIX)] == MACHINE_NAME_PRM_PREFIX:
            machine_name = arg[len(MACHINE_NAME_PRM_PREFIX):]
        if arg[:len(PORT_PRM_PREFIX)] == PORT_PRM_PREFIX:
            server_port = int(arg[len(PORT_PRM_PREFIX):])
        if arg[:len(VERBOSE_PREFIX)] == VERBOSE_PREFIX:
            verbose = (arg[len(VERBOSE_PREFIX):].lower() == "true")

# Start data thread
client_thread: threading.Thread = \
    threading.Thread(target=ping_data, args=("Parameter...",))  # Producer
# print(f"Thread is a {type(client_thread)}")
client_thread.daemon = True  # Dies on exit
client_thread.start()

# Server Initialization
port_number: int = server_port
print("Starting server on port {}".format(port_number))
server = HTTPServer((machine_name, port_number), ServiceHandler)
#
print("Try curl -X GET http://{}:{}{}/oplist".format(machine_name, port_number, PATH_PREFIX))
print("or  curl -v -X VIEW http://{}:{}{} -H \"Content-Length: 1\" -d \"1\"".format(machine_name, port_number,
                                                                                    PATH_PREFIX))
print()
print("or from a browser, http://{}:{}{}/index.html ".format(machine_name, port_number, STATIC_PATH_PREFIX))

#
try:
    server.serve_forever()
except KeyboardInterrupt:
    keep_looping = False
    print("\n\t\tUser interrupted (server.serve), exiting...")
    time.sleep(between_loops * 2)
    print("\n\t\tOver and out!")

print("Done with REST and Web server.")
