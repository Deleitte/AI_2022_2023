import json
from fastapi import FastAPI
from pydantic import BaseModel

from bridge import *

class Data(BaseModel):
    pass

# ser = serial.Serial('/dev/ttyUSB0', 115200)

app = FastAPI()

mock_db = []

@app.get("/")
def hello():
    return {
        "msg": "Hello, World!"
    }
