import json
from time import sleep
from channels.generic.websocket import WebsocketConsumer
import threading
from paho.mqtt import subscribe as subscribe
import pandas as pd
from .ml_model import MLModel
from .monitoring import read_data
from .models import Data_for_Training, Map_Infor
from django.core.cache import cache
import numpy as np
from .live_plot import LivePlot
from .mqtt import Fire_Publisher, User_Publisher
from .apps import client

class ChatConsumer(WebsocketConsumer):
    def connect(self):
        self.accept()
        
        oX=0
        oY=0
        oZ=0
        self.send(json.dumps({'message':oX,'topic':'oX'}))
        self.send(json.dumps({'message':oY,'topic':'oY'}))
        self.send(json.dumps({'message':oZ,'topic':'oZ'}))

        def infiniteloop():
            while True:
                    
                msg=subscribe.simple("data",hostname="192.168.0.100",port=1883)
                message = msg.payload.decode("utf-8")
                split_values = message.split(',')
                oX_char = split_values[3]
                oX = float(oX_char)

                oY_char = split_values[4]
                oY = float(oY_char)

                oZ_char = split_values[5]
                oZ = float(oZ_char)

                self.send(json.dumps({'message':oX,'topic':'oX'}))
                self.send(json.dumps({'message':oY,'topic':'oY'}))
                self.send(json.dumps({'message':oZ,'topic':'oZ'}))
                sleep(0.2)
        thread1 = threading.Thread(target=infiniteloop)
        thread1.start()

class Trainingconsumer(WebsocketConsumer):
    def connect(self):
        self.accept()
    def disconnect(self, close_code):
        print("Disconnected")
    def receive(self, text_data):
        data = json.loads(text_data)
        if data['action'] == "run_model":
            map_obj = cache.get('map_obj')
            number_of_locations = map_obj.total_units
            training_data = pd.DataFrame(list(Data_for_Training.objects.filter(map_infor=map_obj).values('rssi1', 'rssi2', 'rssi3', 'rssi4', 'magy', 'magz', 'location')))
            model = MLModel(training_data=training_data, number_of_locations=number_of_locations)
            a, b = model.train_model()
            model.save_model()
            self.send(text_data=json.dumps({'message': b}))

class Monitoringconsumer(WebsocketConsumer):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.running = False
        self.data = None
        self.axis_x = 0.0
        self.axis_y = 0.0
        self.map = np.ones((10, 10), dtype=int)
        self.plotmap = None
        self.fire_publisher = None
        self.user_publisher = None

    def connect(self):
        self.accept()
        map_obj = cache.get('map_obj')
        if map_obj is None:
            map_obj = Map_Infor.objects.last()
            cache.set('map_obj', map_obj, timeout=None)
        client.publish("real_data_id_14/map_data", str(275) + "," + str(map_obj.walkable_area))
        self.plotmap = LivePlot(map_infor=map_obj, task_number=1)
        self.plotmap.save_fire_size()
        self.fire_publisher = Fire_Publisher(self.plotmap)  # Khởi tạo publisher ngọn lửa
        map_infor_id = map_obj.map_infor_id
        total_units = map_obj.total_units
        area_of_one_unit = map_obj.area_of_one_unit
        area = total_units * area_of_one_unit
        self.send(json.dumps({'message':map_infor_id,'topic':'map_id'}))
        self.send(json.dumps({'message':area,'topic':'map_area'}))
        self.send(json.dumps({'message':total_units,'topic':'unit_number'}))
        walkable_area = map_obj.walkable_area
        ranges = walkable_area.split(',')
        for i in ranges:
            row = (int(i) - 1) // 10
            col = (int(i) - 1) % 10
            self.map[row][col] = 0
        self.data = read_data()
        self.user_publisher = User_Publisher(self.data)  # Truyền instance read_data 
        
        self.data.runmodel(map_obj)

        def infiniteloop():
            while True:
                if self.running:
                    self.send(json.dumps({'message':self.data.opening,'topic':'opening'}))
                    self.send(json.dumps({'message':self.data.mode,'topic':'mode'}))
                    self.send(json.dumps({'message':self.data.x,'topic':'location_x'}))
                    self.send(json.dumps({'message':self.data.y,'topic':'location_y'}))
                    img_data = self.plotmap.generate_next_frame(self.data.x, self.data.y, self.data.heading_angle_radian, self.map, self.data.opening)
                    self.send(json.dumps({'image': img_data, 'topic': 'image'}))
                    self.fire_publisher.publish_fires()
                    self.user_publisher.publish_user()
        thread1 = threading.Thread(target=infiniteloop)
        thread1.start()

    def disconnect(self, close_code):
        print("Disconnected")
        self.running = False
        self.data.end_process()

    def receive(self, text_data):
        data = json.loads(text_data)
        if data['action'] == "initialize":
            self.data.x = float(data['axis_x'])
            self.data.y = float(data['axis_y'])
            self.data.kalman_filter.initialize(np.array([[self.data.x], [self.data.y]]), np.eye(2))
            self.data.reduce_noise.reset_center(self.data.x, self.data.y)
        if data['action'] == "stop":
            self.running = False
            self.data.end_process()
        elif data['action'] == "start":
            self.running = True
            self.data.start_process()

        if data['action'] == "submit_task":
            task_number = int(data['task_number'])
            self.plotmap = LivePlot(map_infor=cache.get('map_obj'), task_number=task_number)
            self.plotmap.save_fire_size()