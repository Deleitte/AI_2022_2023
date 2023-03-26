from __future__ import annotations
from datetime import datetime

import os
import serial
import pydantic
import time
from multiprocessing import Process, Queue
from queue import Empty
from database import get_database
from random import random

from domain import ChangeBrightnessMessage, ESPMessage, KeepAliveMessage, Station, Timeseries


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
        # TODO: Remove this
        if line.decode('utf-8')[0] != '{':
            continue
        line = line.decode('utf-8').strip()
        message = pydantic.parse_raw_as(ESPMessage, line)
        current_time = datetime.now()

        station = db.stations.find_one({"node_id": message.id})
        if not station:
            name = f"Station {message.id}"
            station = Station(node_id=message.id, name=name, last_read=current_time, brightness=0, locked=False, coord_x=message.x, coord_y=message.y)
            db.stations.insert_one(station.dict())
        
        match message:
            case ChangeBrightnessMessage(id=node_id, brightness=brightness, locked=locked):
                timeseries = Timeseries(node_id=node_id, brightness=brightness, locked=locked, timestamp=current_time)
                db.timeseries.insert_one(timeseries.dict())
                db.stations.update_one({"node_id": node_id}, {"$set": {"brightness": brightness, "locked": locked, "last_read": current_time}})

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
        message = None
        if random() > 0.5:
            message = ChangeBrightnessMessage(id=656076313, x=1, y=0, brightness=(int(random() * 4) * 25), locked=(random() > 0.68))
        else:
            message = KeepAliveMessage(id=656076313, x=1, y=0)
        return message.json().encode()

    def write(self, data: str):
        print(f"Wrote {data} to Serial")

