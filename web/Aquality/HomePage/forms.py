from django import forms
import requests

SENSORS = []
def get_ids(): 
    url = "http://127.0.0.1:5000/ids"
    sensors = requests.get(url).json()
    for x in sensors:
        SENSORS.append((str(x), x))

class SensorForm(forms.Form):
    get_ids()
    sensor = forms.ChoiceField(label="Wybierz swoje urządzenie", choices=SENSORS)