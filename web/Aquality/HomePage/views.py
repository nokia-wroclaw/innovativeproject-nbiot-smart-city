from django.shortcuts import render
from django.http import HttpResponse
from .forms import SensorForm
import requests

sensor_id = -1

def index(request):
    if request.method == 'POST':
        form = SensorForm(request.POST)
        if form.is_valid():
            sensor_id = form.cleaned_data['sensor']
            return sensor(request, sensor_id)

    form = SensorForm()
    return render(request, 'form.html', {'form' : form})

def sensor(request, sensor_id):
    url = "http://127.0.0.1:5000/sensor/" + str(sensor_id)
    sensor = requests.get(url).json()
    return render(request, 'sensor.html', {'sensor': sensor})