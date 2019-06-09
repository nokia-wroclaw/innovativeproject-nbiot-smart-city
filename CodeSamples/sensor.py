from flask_restful import Resource
from flask import jsonify
from threading import Lock
import json

mutex = Lock()

sensors = []
IDS = []

def update(thing_id, name, value, IDS): 
    mutex.acquire()
    try:
        rv = False
        if len(sensors) > 0:
            for thing in sensors:
                    if thing["id"] == int(thing_id):
                        thing[name] = float(value.split("'")[1])
                        rv = True
                        break
            if not rv:
                create(thing_id, name, value, IDS)
        else:
            create(thing_id, name, value, IDS)
    finally:
        mutex.release()


def create(thing_id, name, value, IDS):
    IDS.append(int(thing_id))
    IDS = list(set(IDS)).sort()
    thing = {
        "id" : -1,
        "temperature" : 0.0,
        "ph": 7.0,
        "ppm": 0.0,
        "czyst": 0.0
    }
    thing["id"] = int(thing_id)
    thing[name] = float(value.split("'")[1])
    sensors.append(thing)
    print("create sensor id:", thing["id"])

class Sensor(Resource):
    
    def get(self, name):
        for sensor in sensors:
            if name == sensor["id"]:
                return sensor, 200
        return "User not found", 404

    def post(self, name):
        pass
    
    def put(self, name):
        pass

    def delete(self, name):
        pass

class Ids(Resource):
    def get(self):
        if len(IDS) != 0:
            return jsonify(IDS)
        return "No Ids", 404

    def post(self, name):
        pass
    
    def put(self, name):
        pass

    def delete(self, name):
        pass