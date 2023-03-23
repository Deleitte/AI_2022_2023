from __future__ import annotations
from pydantic import BaseModel
import enum

class RType(enum.Enum):
    BRIGHTNESS = 0
    KEEP_ALIVE = 1

# Messages sent to the ESPs
class Command(BaseModel):
    type: int = 0
    action: int


class OnCommand(Command):
    action: int = 1 
    id: int
    brightness: int 


class OffCommand(Command):
    action: int = 0 


# DTOs for the web server's endpoints
class CommandResponse(BaseModel):
    success: bool


class OnRequest(BaseModel):
    id: int
    brightness: int 


class OffRequest(BaseModel):
    id: int

class Station(BaseModel):
    node_id: str
    name: str
    last_read: int
    coord_x: int
    coord_y: int


class Timeseries(BaseModel):
    override: bool
    timestamp: int
    type: RType
    brightness: int
