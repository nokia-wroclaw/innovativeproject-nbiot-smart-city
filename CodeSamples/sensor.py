from flask_restful import Resource

sensors = [
    { 
        "name": "temperature",
        "value": 0.0 
    },
    {
        "name": "ph",
        "value": 0.0
    },
    {
        "name": "ppm",
        "value": 0
    },
    {
        "name": "czyst",
        "value": 0
    }
]

class Sensor(Resource):
    

    def get(self, name):
        for sensor in sensors:
            if name == sensor["name"]:
                return sensor, 200
        return "User not found", 404

    def post(self, name):
        pass
    
    def put(self, name):
        pass

    def delete(self, name):
        pass