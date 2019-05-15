from flask_restful import Resource
from threading import Lock


mutex = Lock()

sensors = []

def update(thing_id, name, value): 
    mutex.acquire()
    try:
        rv = False
        if len(sensors) > 0:
            for thing in sensors:
                    if thing["id"] == int(thing_id):
                        thing[name] = value
                        rv = True
                        break
            if not rv:
                create(thing_id, name, value)
        else:
            create(thing_id, name, value)
    finally:
        mutex.release()

def create(thing_id, name, value):
    thing = {
        "id" : -1,
        "temperature" : 0.0,
        "ph": 7.0,
        "ppm": 0.0,
        "czyst": 0.0
    }
    thing["id"] = int(thing_id)
    thing[name] = value
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