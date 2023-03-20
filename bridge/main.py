import serial
import json

ser = serial.Serial('/dev/ttyUSB0', 115200)

while ser.in_waiting:
    line = ser.readline()
    line = line.decode('utf-8').strip()
    message = json.loads(line)
