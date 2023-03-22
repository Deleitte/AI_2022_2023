from domain import *
from pymongo import MongoClient

def parse(data: dict) -> Station | Timeseries :
        if len(set(data.keys()).difference({"node_id", "name", "last_read", "coord_x", "coord_y"})) == 0:
            return Station(**data)
        elif len(set(data.keys()).difference({"override", "timestamp", "type", "brightness"})) == 0:
            return Timeseries(**data)
        else:
            raise RuntimeError("parse")


class Handlers:

    def __init__(self, mongo_client: MongoClient):
        self.mongo = mongo_client
    
    def handle_station(self, station: Station):
        db = self.mongo.get_database("stargazer_db")
        db.get_collection("stations").insert_one(station.dict())

    def handle_timeseries(self, ts: Timeseries):
        db = self.mongo.get_database("stargazer_db")
        db.get_collection("timeseries").insert_one(ts.dict())
