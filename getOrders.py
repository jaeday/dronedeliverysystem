import time
import pymongo

from pymongo import MongoClient

CONNECTION_STRING = "mongodb+srv://shimmy:Sjh31217@cluster0.fpdlz.mongodb.net/coffeedelivery"

client = MongoClient(CONNECTION_STRING)

print("success!")