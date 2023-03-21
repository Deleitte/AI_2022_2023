import serial
import json
import os
from fastapi import FastAPI

from bridge import *
from domain import *


app = FastAPI()

if os.environ.get("STARGAZER_DEBUG"):
    cereal = MockCereal("/path", 115200)
else:
    cereal = serial.Serial("/dev/TTYUSB0", 115200)

read_channel, write_channel, queue = get_bridge(cereal)

read_channel.start()
write_channel.start()

@app.post("/off")
async def off() -> CommandResponse:
    command = Command(type=0, action=0)
    queue.put(command.json())
    return CommandResponse(success=True)

@app.post("/on")
async def on() -> CommandResponse:
    command = Command(type=0, action=1, brightness=75)
    queue.put(command.json())
    return CommandResponse(success=True)
