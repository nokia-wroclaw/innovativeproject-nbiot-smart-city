import paho.mqtt.client as mqtt
import credentials as cred
import re
from flask import Flask
from flask_restful import Api, Resource, reqparse
import sensor
from threading import Thread

def rest():
    app = Flask(__name__)
    api = Api(app)
    api.add_resource(sensor.Sensor, "/sensor/<int:name>")
    api.add_resource(sensor.Ids, "/ids")
    app.run(debug=True)

def mqtt_fun():
    tokenID         = 0
    specificTopic   = cred.mqtt['token'][tokenID] + "/urn:sku:meter-001/temperature/0/sensorValue"
    generalTopic    = cred.mqtt['token'][tokenID] + "/#"

    def on_connect(client, userdata, flags, rc):
        print("Connected with result code "+str(rc))
        client.subscribe(generalTopic)

    def on_message(client, userdata, msg):
        split = msg.topic.split("/")
        name = split[2]
        thing_id = re.findall("\d+", str(split[1]))[0]
        value = str(msg.payload)
        sensor.update(thing_id, name, value, sensor.IDS)
                
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.username_pw_set(cred.mqtt['login'][tokenID], cred.mqtt['password'][tokenID])
    client.connect(cred.impact['url'], cred.impact['port'], 60)

    client.loop_forever()
tr = Thread(target=mqtt_fun, args=())
tr.start()
rest()