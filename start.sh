#!/bin/bash

python multiple_devices_mocker/mocker.py $1 $2 &
python CodeSamples/mqqt2rest.py &
wait($2 + 2)
python web/Aquality/manage.py runserver &