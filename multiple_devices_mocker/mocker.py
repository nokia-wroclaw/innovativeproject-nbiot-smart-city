import threading
import random
import time
import sys
import paho.mqtt.client as mqtt
import credentials as cred

tokenID = 0
sleep_time = int(sys.argv[2])
metrics = ["temperature", "ph", "ppm", "czyst"]


class myThread (threading.Thread):
    def __init__(self, thread_id):
        threading.Thread.__init__(self)
        self.thread_id = thread_id

    def run(self):
        print("Starting " + str(self.thread_id))
        thread_function(self.thread_id)
        print("Exiting " + str(self.thread_id))


def thread_function(mock_id):
    device_id = str(mock_id).zfill(5)
    topic_base = cred.mqtt['token'][tokenID] + "/urn:sku:meter-" + device_id + "/"
    client = mqtt.Client()
    client.username_pw_set(cred.mqtt['login'][tokenID], cred.mqtt['password'][tokenID])
    while True:
        time.sleep(sleep_time-1)
        client.connect(cred.impact['url'], cred.impact['port'], 60)
        for m in metrics:
            rand_payload = random.randint(0, 512)
            topic_full = topic_base + m + "/0/sensorValue"
            client.publish(topic_full, payload=rand_payload, qos=0, retain=False)
            print(topic_full + " | payl: " + str(rand_payload))
        time.sleep(1)
        client.disconnect()


# Create given number of threads
threads = []
th_amount = int(sys.argv[1])

for x in range(th_amount):
    try:
        temp_thread = myThread(x)
        temp_thread.start()
        threads.append(temp_thread)
    except:
        print("Error: unable to start thread")
for t in threads:
    t.join()
print("Bye")
