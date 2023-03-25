from __future__ import annotations

from datetime import datetime
from typing import Annotated, Literal, List
from pydantic import BaseModel, Field


# Messages sent to the ESPs
class Command(BaseModel):
    type: int = 0
    action: int
    id: int


class OnCommand(Command):
    action: int = 1
    brightness: int 


class OffCommand(Command):
    action: int = 0 


# Messages sent from the ESPs
class BaseESPMessage(BaseModel):
    type: int = 1
    action: int
    id: int
    x: int
    y: int


class KeepAliveMessage(BaseESPMessage):
    action: Literal[0] = 0


class ChangeBrightnessMessage(BaseESPMessage):
    action: Literal[1] = 1
    brightness: int
    overrided: bool


ESPMessage = Annotated[KeepAliveMessage | ChangeBrightnessMessage, Field(discriminator='action')]


# DTOs for the web server's endpoints
class CommandResponse(BaseModel):
    success: bool

class StationsResponse(BaseModel):
    stations: list

class OnRequest(BaseModel):
    id: int
    brightness: int 


class OffRequest(BaseModel):
    id: int

class ChangeNameRequest(BaseModel):
    node_id: int
    name: str

# Database models
class Station(BaseModel):
    node_id: int
    name: str
    last_read: datetime
    brightness: int
    locked: bool
    coord_x: int
    coord_y: int


class Timeseries(BaseModel):
    node_id: int
    locked: bool
    timestamp: datetime
    brightness: int
