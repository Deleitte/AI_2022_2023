import serial
import os
from fastapi import FastAPI

from bridge import *
from domain import *

app = FastAPI()

read_channel, write_channel, queue = get_bridge()

read_channel.start()
write_channel.start()


@app.post("/off")
async def off() -> CommandResponse:
    global queue
    command = Command(type=0, action=0)
    queue.put(command.json())
    return CommandResponse(success=True)


@app.post("/on")
async def on(body: OnRequest) -> CommandResponse:
    global queue
    command = Command(type=0, action=1, brightness=body.brightness)
    queue.put(command.json())
    return CommandResponse(success=True)
