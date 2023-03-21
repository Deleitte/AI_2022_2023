import serial
import json
from fastapi import FastAPI
from dataclasses import dataclass
from multiprocessing import Process, Queue
from pydantic import BaseModel

class Data(BaseModel):
    
    pass

def read_from_cereal(ser: serial.Serial):

    while ser.in_waiting:
        line = ser.readline()
        line = line.decode('utf-8').strip()
        message = json.loads(line)


def write_to_cereal(ser: serial.Serial, queue: Queue):
    while True:
        message = queue.get()
        # write to serial


queue = Queue()
# ser = serial.Serial('/dev/ttyUSB0', 115200)
# bridge = Process(target=append_data, args=(queue,))

app = FastAPI()

mock_db = []

@app.get("/")
def hello():
    return {
        "msg": "Hello, World!"
    }
