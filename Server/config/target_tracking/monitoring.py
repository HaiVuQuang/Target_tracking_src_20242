from .mqtt import mqtt_message_received
from .wbo_filter import WBOFilter
from .kalmanfilter import step_detection
import numpy as np
from .models import Data_for_Training, DeviceInfor
from .ml_model import MLModel
import pandas as pd
from django.core.cache import cache
from .kalmanfilter import linear_kalman_filter, restricted_area
import time
from django.utils import timezone
import os

class read_data:
    def __init__(self):
        self.opening = 100
        self.mode = 0
        self.rssi1 = -100
        self.rssi2 = -100
        self.rssi3 = -100
        self.rssi4 = -100
        self.magy = 0.0
        self.magz = 0.0
        self.step_detection = step_detection(0.9, -0.9, 100, 600)
        self.model = None
        self.accz = 0.0
        self.time = 0.0
        self.time_start = None
        self.time_end = None
        self.offset_angle = 0
        self.angle = 0.0
        self.heading_angle_radian = 0.0
        self.x = 0.0
        self.y = 0.0
        self.step_counter = 0
        self.map_obj = cache.get('map_obj')
        self.kalman_filter = linear_kalman_filter(F=np.eye(2), H=np.eye(2), Q=np.eye(2), R=np.eye(2))
        self.reduce_noise = restricted_area()

    def runmodel(self, map_obj=None):
        number_of_locations = map_obj.total_units
        training_data = pd.DataFrame(list(Data_for_Training.objects.filter(map_infor=map_obj).values('rssi1', 'rssi2', 'rssi3', 'rssi4','magy', 'magz', 'location')))
        self.model = MLModel(training_data=training_data, number_of_locations=number_of_locations)
        self.model.load_model()

    def start_process(self): 
        mqtt_message_received.connect(self.process_message, sender=None)
        self.time_start = time.time()
        print(mqtt_message_received.receivers)

    def process_message(self, sender, payload, **kwargs):
        data = payload.split(',')
        self.accz = float(data[6])
        self.angle = float(data[13])
        self.heading_angle_radian = np.deg2rad(self.angle-self.offset_angle)
        self.time =int(data[18])
        self.time_end = time.time()
        rssi1_char = (data[0])
        rssi2_char = (data[1])
        rssi3_char = (data[2])
        rssi4_char = (data[3])
        magy_char = (data[8])
        magz_char = (data[9])
        self.opening = int(data[16])

    
        if (rssi1_char) != self.rssi1 or (rssi2_char) != self.rssi2 or (rssi3_char) != self.rssi3 or (rssi4_char) != self.rssi4:
            self.rssi1 = (rssi1_char)
            self.rssi2 = (rssi2_char)
            self.rssi3 = (rssi3_char)
            self.rssi4 = (rssi4_char)
            self.magy = (magy_char)
            self.magz = (magz_char)
            DeviceInfor.objects.create(
                rssi1 = self.rssi1,
                rssi2 = self.rssi2,
                rssi3 = self.rssi3,
                rssi4 = self.rssi4,
                accx = 0,
                accy = 0,
                accz = self.accz,
                magx = 0,
                magy = self.magy,
                magz = self.magz,
                gyrox = 0,
                gyroy = 0,
                gyroz = 0,
                eulerx = self.angle,
                eulery = 0,
                eulerz = 0,
                date_created = timezone.now(),
            )
        
        if self.step_detection.detect_step(self.accz, self.time):
            self.time_start = time.time()
            self.step_counter += 1
            print('step_counter:', self.step_counter)
            # Fetch the 10 latest RSSI values from the DeviceInfor table
            latest_rssi_values = DeviceInfor.objects.order_by('-date_created')[:10].values(
                'rssi1', 'rssi2', 'rssi3', 'rssi4', 'magy', 'magz'
            )
            # Convert the queryset to a numpy array for further processing
            latest_rssi_array = np.array([[item['rssi1'], item['rssi2'], item['rssi3'], item['rssi4'], item['magy'], item['magz']]  for item in latest_rssi_values])
            min_value = np.min(latest_rssi_array, axis=0)
            max_value = np.max(latest_rssi_array, axis=0)
            wbo_filters = [WBOFilter(int(min_value[i]), int(max_value[i])) for i in range(4)]            
            latest_rssi_array = latest_rssi_array.T
            # for j in range(10):
            #     latest_rssi_array[0][9 - j] = wbo_filters[0].proposed_filter(int(round(latest_rssi_array[0][9 - j])))
            #     latest_rssi_array[1][9 - j] = wbo_filters[1].proposed_filter(int(round(latest_rssi_array[1][9 - j])))
            #     latest_rssi_array[2][9 - j] = wbo_filters[2].proposed_filter(int(round(latest_rssi_array[2][9 - j])))
            #     latest_rssi_array[3][9 - j] = wbo_filters[3].proposed_filter(int(round(latest_rssi_array[3][9 - j])))
            # Reshape the array to match the input shape of the model
            latest_rssi_array = latest_rssi_array[:, ::-1]
            lastest_rssi_array = latest_rssi_array.reshape(1, 6, 10)
            data = (lastest_rssi_array - self.model.min)/(self.model.max - self.model.min)
            data = data.reshape(1, 6, 10, 1)
            # prediction = self.model.model.predict(data)
            # label = prediction.argmax(axis=1)[0] + 1
            # print(label)
            # values = Data_for_Training.objects.filter(map_infor=self.map_obj, location=label).values('axis_x', 'axis_y')
            # self.x = values.first()['axis_x']
            # self.y = values.first()['axis_y']
            # self.reduce_noise.reset_center(0.6*self.axis_x, 0.6*self.axis_y)
            # x, y = self.reduce_noise.set_positon(values.first()['axis_x'], values.first()['axis_y'])
            # # self.kalman_filter.update(np.array([[values.first()['axis_x']], [values.first()['axis_y']]]), control_input=np.array([[0.6*self.axis_x], [0.6*self.axis_y]]))
            # # self.x = self.kalman_filter.state[0][0]
            # # self.y = self.kalman_filter.state[1][0]
            # if x != None and y != None:
            #     self.x = x
            #     self.y = y
            self.x = self.x + 0.6*np.sin(self.heading_angle_radian)
            self.y = self.y + 0.6*np.cos(self.heading_angle_radian)


        # if ((self.time_end - self.time_start) >= 2):
        #     self.time_start = time.time()
        #     # Fetch the 10 latest RSSI values from the DeviceInfor table
        #     latest_rssi_values = DeviceInfor.objects.order_by('-date_created')[:10].values(
        #         'rssi1', 'rssi2', 'rssi3', 'rssi4', 'magy', 'magz'
        #     )
        #     # Convert the queryset to a numpy array for further processing
        #     latest_rssi_array = np.array([[item['rssi1'], item['rssi2'], item['rssi3'], item['rssi4'], item['magy'], item['magz']] for item in latest_rssi_values])
        #     min_value = np.min(latest_rssi_array, axis=0)
        #     max_value = np.max(latest_rssi_array, axis=0)
        #     wbo_filters = [WBOFilter(int(min_value[i]), int(max_value[i])) for i in range(4)]
        #     latest_rssi_array = latest_rssi_array.T
        #     # for j in range(10):
        #     #     latest_rssi_array[0][9 - j] = wbo_filters[0].proposed_filter(int(round(latest_rssi_array[0][9 - j])))
        #     #     latest_rssi_array[1][9 - j] = wbo_filters[1].proposed_filter(int(round(latest_rssi_array[1][9 - j])))
        #     #     latest_rssi_array[2][9 - j] = wbo_filters[2].proposed_filter(int(round(latest_rssi_array[2][9 - j])))
        #     #     latest_rssi_array[3][9 - j] = wbo_filters[3].proposed_filter(int(round(latest_rssi_array[3][9 - j])))
        #     # Reshape the array to match the input shape of the model
        #     latest_rssi_array = latest_rssi_array[:, ::-1]
        #     print(latest_rssi_array)
        #     lastest_rssi_array = latest_rssi_array.reshape(1, 6, 10)
        #     data = (lastest_rssi_array - self.model.min)/(self.model.max - self.model.min)
        #     data = data.reshape(1, 6, 10, 1)
        #     prediction = self.model.model.predict(data)
        #     label = prediction.argmax(axis=1)[0] + 1
        #     print(label)
        #     values = Data_for_Training.objects.filter(map_infor=self.map_obj, location=label).values('axis_x', 'axis_y')
        #     # x, y = self.reduce_noise.set_positon(values.first()['axis_x'], values.first()['axis_y'])
        #     # # self.kalman_filter.update(np.array([[values.first()['axis_x']], [values.first()['axis_y']]]), control_input=np.array([[0*self.axis_x], [0*self.axis_y]]))
        #     # # self.x = self.kalman_filter.state[0][0]
        #     # # self.y = self.kalman_filter.state[1][0]
        #     # if x != None and y != None:
        #     #     self.x = x
        #     #     self.y = y
        #     self.x = values.first()['axis_x']
        #     self.y = values.first()['axis_y']

    def end_process(self):
        mqtt_message_received.disconnect(self.process_message)






















# class read_data:
#     def __init__(self, opening, mode):
#         self.rssi1 = None
#         self.rssi2 = None
#         self.rssi3 = None
#         self.rssi4 = None
#         self.opening = opening
#         self.mode = mode
#         self.pre_rssi1 = -100
#         self.pre_rssi2 = -100
#         self.pre_rssi3 = -100
#         self.pre_rssi4 = -100
#         self.rssi1_filter = WBOFilter(-70, -10)
#         self.rssi2_filter = WBOFilter(-70, -10)
#         self.rssi3_filter = WBOFilter(-70, -10)
#         self.rssi4_filter = WBOFilter(-70, -10)
#         self.step_detection = step_detection(1.0, -1.0, 100, 600)
#         self.model = None
#         self.min = None
#         self.max = None
#         self.accz = 0.0
#         self.time = 0.0
#         self.time_start = None
#         self.time_end = None
#         self.offset_angle = 90 + 90 + 180
#         self.angle = 0.0
#         self.axis_x = 0.0
#         self.axis_y = 0.0
#         self.x = 0.0
#         self.y = 0.0
#         self.step_counter = 0
#         self.map_obj = cache.get('map_obj')
#         self.kalman_filter = linear_kalman_filter(F=np.eye(2), H=np.eye(2), Q=2*np.eye(2), R=np.eye(2))
#         self.reduce_noise = restricted_area()

#     def runmodel(self):
#         map_obj = cache.get('map_obj')
#         number_of_locations = map_obj.total_units
#         rssi_data = pd.DataFrame(list(Data_for_Training.objects.filter(map_infor=map_obj).values('rssi1', 'rssi2', 'rssi3', 'rssi4', 'location')))
#         rssi_data[['rssi1', 'rssi2', 'rssi3', 'rssi4']] = rssi_data[['rssi1', 'rssi2', 'rssi3', 'rssi4']].astype(int)
#         self.model = MLModel(rssi_data=rssi_data, number_of_locations=number_of_locations)
#         self.model.load_model()
#         # BASE_DIR = os.path.dirname(os.path.abspath(__file__))
#         # file_path = os.path.join(BASE_DIR, 'rssi_data/WBOrssi.csv')
#         # df = pd.read_csv(file_path)
#         input_colums = ['rssi1', 'rssi2', 'rssi3', 'rssi4']
#         X = rssi_data[input_colums]#edited
#         X = np.array(X)
#         self.min = np.min(X, axis=0)
#         self.max = np.max(X, axis=0)

#     def start_process(self): 
#         mqtt_message_received.connect(self.process_message, sender=None)
#         self.time_start = time.time()
#         print(mqtt_message_received.receivers)

#     def process_message(self, sender, payload, **kwargs):
#         data = payload.split(',')
#         self.accz = float(data[9])
#         self.angle = float(data[4])
#         radian = np.deg2rad(self.angle-self.offset_angle)
#         self.axis_x = np.sin(radian)
#         self.axis_y = np.cos(radian)
#         self.time =int(data[10])
#         self.time_end = time.time()
#         rssi1_char = (data[0])
#         rssi2_char = (data[1])
#         rssi3_char = (data[2])
#         rssi4_char = (data[3])


#         if (rssi1_char) != self.rssi1 or (rssi2_char) != self.rssi2 or (rssi3_char) != self.rssi3 or (rssi4_char) != self.rssi4:
#             if self.step_detection.detect_step(self.accz, self.time):
#                 self.step_counter += 1
#                 print('step_counter:', self.step_counter)
#                 self.time_start = time.time()
#                 self.rssi1_filter = WBOFilter(-70, -10)
#                 self.rssi2_filter = WBOFilter(-70, -10)
#                 self.rssi3_filter = WBOFilter(-70, -10)
#                 self.rssi4_filter = WBOFilter(-70, -10)
#                 self.rssi1 = float(rssi1_char)
#                 self.rssi2 = float(rssi2_char)
#                 self.rssi3 = float(rssi3_char)
#                 self.rssi4 = float(rssi4_char)
#                 data = np.array([[self.rssi1, self.rssi2, self.rssi3, self.rssi4]])
#                 data = (data - self.min) / (self.max - self.min)
#                 data = np.array(data).reshape(1, -1)
#                 prediction = self.model.model.predict(data)
#                 label = prediction.argmax(axis=1)[0] + 1
#                 values = Data_for_Training.objects.filter(map_infor=self.map_obj, location=label).values('axis_x', 'axis_y')
#                 # self.kalman_filter.update(np.array([[values.first()['axis_x']], [values.first()['axis_y']]]), control_input=np.array([[0.6*self.axis_x], [0.6*self.axis_y]]))
#                 # self.x = self.kalman_filter.state[0][0]
#                 # self.y = self.kalman_filter.state[1][0]
#                 # self.reduce_noise.reset_center(self.x + 0.5*self.axis_x, self.y + 0.5*self.axis_y)
#                 # x, y = self.reduce_noise.set_positon(values.first()['axis_x'], values.first()['axis_y'])
#                 # if x != None and y != None:
#                 #     self.x = x
#                 #     self.y = y
#                 self.x = values.first()['axis_x']
#                 self.y = values.first()['axis_y']


#             elif (self.time_end - self.time_start) >= 0.25:
#                 self.time_start = time.time()
#                 # self.rssi1_filter = WBOFilter(-70, -10)
#                 # self.rssi2_filter = WBOFilter(-70, -10)
#                 # self.rssi3_filter = WBOFilter(-70, -10)
#                 # self.rssi4_filter = WBOFilter(-70, -10)
#                 self.rssi1 = float(rssi1_char)
#                 self.rssi2 = float(rssi2_char)
#                 self.rssi3 = float(rssi3_char)
#                 self.rssi4 = float(rssi4_char)
#                 data = np.array([[self.rssi1, self.rssi2, self.rssi3, self.rssi4]])
#                 data = (data - self.min) / (self.max - self.min)
#                 data = np.array(data).reshape(1, -1)
#                 prediction = self.model.model.predict(data)
#                 label = prediction.argmax(axis=1)[0] + 1
#                 values = Data_for_Training.objects.filter(map_infor=self.map_obj, location=label).values('axis_x', 'axis_y')
#                 # self.kalman_filter.update(np.array([[values.first()['axis_x']], [values.first()['axis_y']]]), control_input=np.array([[0*self.axis_x], [0*self.axis_y]]))
#                 # self.x = self.kalman_filter.state[0][0]
#                 # self.y = self.kalman_filter.state[1][0]
#                 # x, y = self.reduce_noise.set_positon(values.first()['axis_x'], values.first()['axis_y'])
#                 # if x != None and y != None:
#                 #     self.x = x
#                 #     self.y = y
#                 self.x = values.first()['axis_x']
#                 self.y = values.first()['axis_y']

#     def end_process(self):
#         mqtt_message_received.disconnect(self.process_message)
    