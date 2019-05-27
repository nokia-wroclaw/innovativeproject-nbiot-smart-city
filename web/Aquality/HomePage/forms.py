from django import forms
import requests

url = "http://127.0.0.1:5000/ids"
sensors = requests.get(url).json()
SENSORS = []
for x in sensors:
    SENSORS.append((str(x), x))

class SensorForm(forms.Form):
    sensor = forms.ChoiceField(label="Wybierz swoje urządzenie", choices=SENSORS)