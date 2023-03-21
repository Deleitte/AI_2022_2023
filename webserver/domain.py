from pydantic import BaseModel

class Command(BaseModel):
    type: int
    action: int
    brightness: int | None

class CommandResponse(BaseModel):
    success: bool

