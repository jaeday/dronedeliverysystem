import time
import os
import pymongo
import json

from pymongo import MongoClient

CONNECTION_STRING = "mongodb+srv://shimmy:Sjh31217@cluster0.fpdlz.mongodb.net/coffeedelivery?ssl=true&ssl_cert_reqs=CERT_NONE"

destList = {}

try:
    while True:
        time.sleep(900)
        client = MongoClient(CONNECTION_STRING)
        db = client.coffeedelivery
        collection = db.orders
        for order in collection.find():
            if order['isDelivered'] == False:
                orderLocation = order['shippingAddress']
                destList['orderLocation'] = order['orderItems']
                collection.update({"isDelivered": "False"}, {"$set" : "True"})
  
        with open('deliveryOne', 'w') as convert_file:
            convert_file.write(json.dumps(destList))
        

except KeyboardInterrupt:
    print("Press Ctrl-C to terminate the process")
    pass

