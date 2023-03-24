from __future__ import annotations
from datetime import datetime

import os
import serial
import pydantic
import time
from multiprocessing import Process, Queue
from queue import Empty
from database import get_database

from domain import ChangeBrightnessMessage, ESPMessage, KeepAliveMessage, Timeseries


def create_cereal() -> serial.Serial | MockCereal:
    if os.environ.get("STARGAZER_DEBUG"):
        return MockCereal("/path", 115200)
    else:
        return serial.Serial(os.environ["STARGAZER_CEREAL"], 115200)


def read_from_cereal():
    ser = create_cereal()
    db = get_database()

    while True:
        line = ser.readline()
        print(line)
        # TODO: Remove this
        if line.decode('utf-8')[0] != '{':
            continue
        line = line.decode('utf-8').strip()
        message = pydantic.parse_raw_as(ESPMessage, line) 
        match message:
            case ChangeBrightnessMessage(id=node_id, brightness=brightness, overrided=overrided):
                timeseries = Timeseries(node_id=node_id, brightness=brightness, override=overrided, timestamp=datetime.now())
                db.timeseries.insert_one(timeseries.dict())


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
        time.sleep(10)
        message = KeepAliveMessage(id=214, x=5, y=8)
        return message.json().encode()

    def write(self, data: str):
        print(f"Wrote {data} to Serial")

