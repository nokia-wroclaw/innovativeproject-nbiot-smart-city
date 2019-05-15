# Example for an MQTT topic subscription in Nokia IMPACT IoT using paho.mqtt.client
# based on an example from https://pypi.org/project/paho-mqtt/#usage-and-api
#
# Note: credentials.py are not shared on github and are in .gitignore

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
    app.run(debug=False)

def mqtt_fun():
    tokenID         = 0
    specificTopic   = cred.mqtt['token'][tokenID] + "/urn:sku:meter-001/temperature/0/sensorValue"
    generalTopic    = cred.mqtt['token'][tokenID] + "/#"

    # The callback for when the client receives a CONNACK response from the server.
    def on_connect(client, userdata, flags, rc):
        print("Connected with result code "+str(rc))

        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        client.subscribe(generalTopic)

    # The callback for when a PUBLISH message is received from the server.
    def on_message(client, userdata, msg):
        split = msg.topic.split("/")
        name = split[2]
        thing_id = re.findall("\d+", str(split[1]))[0]
        value = str(msg.payload)
        sensor.update(thing_id, name, value)
                
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.username_pw_set(cred.mqtt['login'][tokenID], cred.mqtt['password'][tokenID])

    client.connect(cred.impact['url'], cred.impact['port'], 60)

    client.loop_forever()
tr = Thread(target=mqtt_fun, args=())
tr.start()
rest()

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.