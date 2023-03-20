import serial
from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import time

hostName = "192.168.1.140"
serverPort = 8080
ser = serial.Serial('/dev/ttyUSB0', 115200)
stations = {}

class Station:
    def __init__(self, id, x, y, brightness):
        self.id = id
        self.x = x
        self.y = y
        self.brightness = brightness

def updateStatus():
    while ser.in_waiting:
        line = ser.readline()
        print(line)
        line = line.decode('utf-8').strip()
        message = json.loads(line)
        if message['id'] not in stations:
            stations[message['id']] = Station(message['id'], message['x'], message['y'], 0)  
        if message['type'] == 1:
            stations[message['id']].brightness = 100          
        if message['type'] == 2:
            print(int(message['brightness']))
            stations[message['id']].brightness = message['brightness']
                

class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()

        updateStatus()
        self.wfile.write(bytes("<html><head><title>Smart City Lights</title><h1>Station Status</h1></head>", "utf-8"))
        self.wfile.write(bytes("<body>", "utf-8"))
        for station in stations:
            self.wfile.write(bytes("<p>Station: %s, x: %s, y: %s, brightness: %s</p>" % (stations[station].id, stations[station].x, stations[station].y, stations[station].brightness), "utf-8"))
        self.wfile.write(bytes("</body></html>", "utf-8"))

if __name__ == "__main__":        
    webServer = HTTPServer((hostName, serverPort), MyServer)
    print("Server started http://%s:%s" % (hostName, serverPort))

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")