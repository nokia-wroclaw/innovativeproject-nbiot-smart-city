import requests

urlPushGateway = "http://localhost:9091/"

sensorID    = "temp-005"
urlSensor   = urlPushGateway + "metrics/job/" + sensorID

sensorValue = "temperature 5\n"

r = requests.post(urlSensor, data=sensorValue)
