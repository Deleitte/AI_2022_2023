from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from pydantic import parse_obj_as
from typing import List

from bridge import *
from domain import *

if not os.environ.get("STARGAZER_DEBUG"):
    devices = filter(lambda name: 'ttyUSB' in name, os.listdir('/dev'))
    for device in devices:
        print("Connecting to device", device)
        os.environ["STARGAZER_CEREAL"] = f"/dev/{device}"
        break
    else:
        print("No device has been found, debug mode has been activated")
        os.environ["STARGAZER_DEBUG"] = "1"

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


@app.get("/stations")
async def stations() -> StationsResponse:
    db = get_database()
    stations = list(db.stations.find())
    return StationsResponse(stations=parse_obj_as(List[Station], stations))


@app.get("/stations/{node_id}")
async def station(node_id: int) -> Station:
    db = get_database()
    station = db.stations.find_one({ "node_id": node_id })
    return Station.parse_obj(station) 
