from pydantic import BaseModel


class Command(BaseModel):
    type: int
    action: int
    brightness: int | None = None


class CommandResponse(BaseModel):
    success: bool


class OnRequest(BaseModel):
    brightness: int 

