from __future__ import annotations
from random import randint
from multiprocessing import Process, Queue
from queue import Empty
import serial
import json


def read_from_cereal(ser: serial.Serial | MockCereal):
    cereal = serial.Serial('/dev/ttyUSB0', 115200)
    while True:
        line = cereal.readline()
        # TODO: Remove this
        if line.decode('utf-8')[0] != '{':
            continue
        line = line.decode('utf-8').strip()
        message = json.loads(line)


def write_to_cereal(ser: serial.Serial | MockCereal, queue: Queue):
    cereal = serial.Serial('/dev/ttyUSB0', 115200)
    while True:
        try:
            message = queue.get(timeout=1000).encode('utf-8')
            cereal.write(message)
        except Empty:
            pass


def get_bridge(ser: serial.Serial | MockCereal) -> tuple[Process, Process, Queue]:
    queue = Queue()
    write_process = Process(target=write_to_cereal, args=(ser, queue))
    read_process = Process(target=read_from_cereal, args=(ser,))
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

