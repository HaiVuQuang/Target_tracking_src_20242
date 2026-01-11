import random
from paho.mqtt import client as mqtt_client
from django.dispatch import Signal

mqtt_message_received = Signal()

broker = '192.168.0.100'
port = 1883
client_id = f'subscribe-{random.randint(0, 100)}'

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
        client.subscribe("training_data_id_14")
    else:
        print("Failed to connect, return code %d\n", rc)

def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    mqtt_message_received.send(sender=None, payload=payload)
def connect_mqtt() -> mqtt_client:
    client = mqtt_client.Client(mqtt_client.CallbackAPIVersion.VERSION1, client_id)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(broker, port)
    client.loop_start()
    return client


#Xử lý gửi cập nhật thông tin ngọn lửa 
class Fire_Publisher:
    def __init__(self, fire_information=None):
        self.broker = 'localhost'
        self.port = 1883
        self.topic = "real_data_id_14/fire_data"
        self.fires = fire_information
        self.client = self.connect_mqtt()

    def connect_mqtt(self):
        def on_connect(client, userdata, flags, rc):
            if rc == 0:
                print("Connected to MQTT Broker!")
            else:
                print("Failed to connect, return code %d\n", rc)
        client_id = f'subscribe-{random.randint(0, 100)}'
        client = mqtt_client.Client(mqtt_client.CallbackAPIVersion.VERSION1, client_id)
        client.on_connect = on_connect
        client.connect(self.broker, self.port)
        client.loop_start()
        return client

    def publish_fires(self):
        #id1,x1,y1,lvl1,id2,x2,y2,lvl2,...
        message_parts = []
        for fire in self.fires.fire:
            message_parts.extend([
                str(fire.fires_location_id),
                str(fire.fires_axis_x),
                str(fire.fires_axis_y),
                str(self.fires.level[fire.fires_location_id])
            ])
        
        message = ",".join(message_parts)
        result = self.client.publish(self.topic, message)
        if result.rc != 0:
            print("Failed to publish fires data")

    def stop(self):
        self.client.loop_stop()
        self.client.disconnect()


#Xử lý gửi cập nhật thông tin user 
class User_Publisher:
    def __init__(self, read_data_instance=None):
        self.broker = 'localhost'
        self.port = 1883
        self.topic = "real_data_id_14/user_data"
        self.client = self.connect_mqtt()
        self.data = read_data_instance  

    def connect_mqtt(self):
        def on_connect(client, userdata, flags, rc):
            if rc == 0:
                print("Connected to MQTT Broker!")
            else:
                print("Failed to connect, return code %d\n", rc)
        client_id = f'subscribe-{random.randint(0, 100)}'
        client = mqtt_client.Client(mqtt_client.CallbackAPIVersion.VERSION1, client_id)
        client.on_connect = on_connect
        client.connect(self.broker, self.port)
        client.loop_start()
        return client
    
    def publish_user(self):  
        message = str(self.data.x) + "," + str(self.data.y) + "," + str(500)
        result = self.client.publish(self.topic, message)
        if result.rc != 0:
            print("Failed to publish user data", result.rc)

    def stop(self):
        self.client.loop_stop()
        self.client.disconnect() 









