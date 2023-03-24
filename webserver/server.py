from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from bridge import *
from domain import *

app = FastAPI()

origins = ["*"]
app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

read_channel, write_channel, queue = get_bridge()

read_channel.start()
write_channel.start()


@app.post("/off")
async def off(body: OffRequest) -> CommandResponse:
    global queue
    command = OffCommand(id=body.id)
    queue.put(command.json())
    return CommandResponse(success=True)


@app.post("/on")
async def on(body: OnRequest) -> CommandResponse:
    global queue
    command = OnCommand(id=body.id, brightness=body.brightness)
    print(command.json())
    queue.put(command.json())
    return CommandResponse(success=True)

