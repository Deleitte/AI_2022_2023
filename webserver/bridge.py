from __future__ import annotations
import os
import serial
import json
from random import randint
from multiprocessing import Process, Queue
from queue import Empty


def create_cereal():
    if os.environ.get("STARGAZER_DEBUG"):
        return MockCereal("/path", 115200)
    else:
        return serial.Serial('/dev/ttyUSB0', 115200)


def read_from_cereal():
    ser = create_cereal() 
    while True:
        line = ser.readline()
        print(line)
        # TODO: Remove this
        if line.decode('utf-8')[0] != '{':
            continue
        line = line.decode('utf-8').strip()
        message = json.loads(line)


def write_to_cereal(queue: Queue):
    ser = create_cereal() 
    while True:
        try:
            message = queue.get(timeout=1000).encode('utf-8')
            ser.write(message)
        except Empty:
            pass


def get_bridge() -> tuple[Process, Process, Queue]:
    queue = Queue()
    write_process = Process(target=write_to_cereal, args=(queue,))
    read_process = Process(target=read_from_cereal)
    return (read_process, write_process, queue)


class MockCereal:
    def __init__(self, path: str, baudrate: int):
        self.path = path
        self.in_waiting = True
        self.baudrate = baudrate

    def readline(self) -> bytes:
        return json.dumps({
            "state": "on" if randint(0, 1) else "off"
            }).encode()

    def write(self, data: str):
        print(f"Wrote {data} to Serial")

