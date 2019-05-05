# Example for an MQTT topic subscription in Nokia IMPACT IoT using paho.mqtt.client
# based on an example from https://pypi.org/project/paho-mqtt/#usage-and-api
#
# Note: credentials.py are not shared on github and are in .gitignore

import paho.mqtt.client as mqtt
import credentials as cred

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
    print(msg.topic+" "+str(msg.payload))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(cred.mqtt['login'][tokenID], cred.mqtt['password'][tokenID])

client.connect(cred.impact['url'], cred.impact['port'], 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()