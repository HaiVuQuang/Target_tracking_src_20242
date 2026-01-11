from .mqtt import mqtt_message_received
from .models import Data_for_Training
from django.utils import timezone

class collect_data:
    def __init__(self, location, axis_x, axis_y, number_of_rssi, map_obj):
        self.location = location
        self.axis_x = axis_x
        self.axis_y = axis_y
        self.number_of_rssi = number_of_rssi
        self.map_obj = map_obj
        self.pre_rssi1 = 0
        self.pre_rssi2 = 0
        self.pre_rssi3 = 0
        self.pre_rssi4 = 0
        self.counter = 0
        mqtt_message_received.connect(self.process_message, sender=None)
        print(mqtt_message_received.receivers)

    def process_message(self, sender, payload, **kwargs):
        data = payload.split(',')
        rssi1 = data[0]
        rssi2 = data[1]
        rssi3 = data[2]
        rssi4 = data[3]
        magy = data[4]
        magz = data[5]
        if int(rssi1) >= -75 and int(rssi2) >= -75 and int(rssi3) >= -75 and int(rssi4) >= -75:
            if rssi1 != self.pre_rssi1 or rssi2 != self.pre_rssi2 or rssi3 != self.pre_rssi3 or rssi4 != self.pre_rssi4:
                Data_for_Training.objects.create(
                    map_infor = self.map_obj,
                    rssi1 = rssi1,
                    rssi2 = rssi2,
                    rssi3 = rssi3,
                    rssi4 = rssi4,
                    magy = magy,
                    magz = magz,
                    location = self.location,
                    axis_x = self.axis_x,
                    axis_y = self.axis_y,
                    date_created = timezone.now()
                )
                self.pre_rssi1 = rssi1
                self.pre_rssi2 = rssi2
                self.pre_rssi3 = rssi3
                self.pre_rssi4 = rssi4
                self.counter += 1
                print(self.counter)
                if self.counter == self.number_of_rssi:
                    mqtt_message_received.disconnect(self.process_message)
                    return
    def check(self):
        return self.counter