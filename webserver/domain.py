from pydantic import BaseModel


# Messages sent to the ESPs
class Command(BaseModel):
    type: int = 0
    action: int


class OnCommand(Command):
    action: int = 1 
    brightness: int 


class OffCommand(Command):
    action: int = 0 


# DTOs for the web server's endpoints
class CommandResponse(BaseModel):
    success: bool


class OnRequest(BaseModel):
    brightness: int 

