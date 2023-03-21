import serial
import json
from multiprocessing import Process, Queue

def read_from_cereal(ser: serial.Serial):
    while ser.in_waiting:
        line = ser.readline()
        line = line.decode('utf-8').strip()
        message = json.loads(line)


def write_to_cereal(ser: serial.Serial, queue: Queue):
    while True:
        message = queue.get()


def get_bridge(ser: serial.Serial) -> tuple[Process, Process, Queue]:
    queue = Queue()
    write_process = Process(target=write_to_cereal, args=(ser, queue))
    read_process = Process(target=read_from_cereal, args=(ser,))
    return (read_process, write_process, queue)
