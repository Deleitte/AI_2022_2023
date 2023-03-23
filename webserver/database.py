import os
from pymongo import MongoClient
from pymongo.database import Database


def get_database() -> Database:
    uri = os.environ.get('STARGAZER_MONGO_URL') or 'mongodb://star:drive@localhost:27017/'
    client = MongoClient(uri)
    return client.stargazer
