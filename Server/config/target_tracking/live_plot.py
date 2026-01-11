import matplotlib
matplotlib.use('Agg')
from matplotlib import patches
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from matplotlib.offsetbox import OffsetImage, AnnotationBbox
import io
import math
import base64
import os
from .models import Fires_Information

map_size = 10
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
file_path = os.path.join(BASE_DIR, 'static/image/fire.png')

class LivePlot:
    def __init__(self, map_infor=None, task_number=1):
        self.fire = Fires_Information.objects.filter(map_infor=map_infor, task_number=task_number)
        self.size = {fires.fires_location_id: 0 for fires in self.fire}
        self.level = {fires.fires_location_id: fires.fires_size for fires in self.fire}
        self.map_infor = map_infor
        self.task_number = task_number

    def is_fire_in_view_cone(self, user_x, user_y, heading_angle_radians, fire_x, fire_y, half_fov_deg, view_distance):
        #Check khoảng cách
        dist_x = fire_x - user_x
        dist_y = fire_y - user_y
        dist = math.sqrt(dist_x**2 + dist_y**2)
        if dist > view_distance: 
            return False

        #Tính góc giữa user và fire, trong hệ tọa độ của Wedge (Đồng bộ với vẽ nón nhìn)
        #Góc 0° của Wedge là trục X dương, góc 90° là hướng lên trên
        if dist_x == 0:
            fire_angle = 90 if dist_y > 0 else 270
        else:
            fire_angle = math.degrees(math.atan2(dist_y, dist_x))
            if fire_angle < 0:
                fire_angle += 360
                
        heading_deg = math.degrees(heading_angle_radians)
        if heading_deg < 0:
            heading_deg += 360
        #Chuyển về hệ tọa độ của Wedge
        heading_deg = 90 - heading_deg
        if heading_deg < 0:
            heading_deg += 360
        #Tính độ lệch góc và đưa về -180 -> 180
        angle_diff = fire_angle - heading_deg
        angle_diff = (angle_diff + 180) % 360 - 180
        return abs(angle_diff) <= half_fov_deg

    def save_fire_size(self):
        for f in self.fire:
            if f.fires_size == 1:
                self.size[f.fires_location_id] = 0.015
            elif f.fires_size == 2:
                self.size[f.fires_location_id] = 0.023
            elif f.fires_size == 3:
                self.size[f.fires_location_id] = 0.03
            elif f.fires_size == 4:
                self.size[f.fires_location_id] = 0.04
            elif f.fires_size == 5:
                self.size[f.fires_location_id] = 0.05
            elif f.fires_size == 0:
                self.size[f.fires_location_id] = 0        

    def generate_next_frame(self, x=0, y=0, heading_angle_radians=0, map=None, valve_open_status=0):
        fig, ax = plt.subplots(figsize=(5, 5))
        half_fov_deg = 30
        view_distance = 1.5
        for f in self.fire:
            is_in_view = self.is_fire_in_view_cone(x, y, heading_angle_radians, f.fires_axis_x, f.fires_axis_y, half_fov_deg, view_distance)
            if is_in_view:
                print(valve_open_status)
                if valve_open_status >= 3 and valve_open_status < 20:
                    self.size[f.fires_location_id] = self.size[f.fires_location_id] - 0.001
                elif valve_open_status >= 20 and valve_open_status < 40:
                    self.size[f.fires_location_id] = self.size[f.fires_location_id] - 0.002
                elif valve_open_status >= 40 and valve_open_status < 60:
                    self.size[f.fires_location_id] = self.size[f.fires_location_id] - 0.003
                elif valve_open_status >= 60 and valve_open_status < 80:
                    self.size[f.fires_location_id] = self.size[f.fires_location_id] - 0.004
                elif valve_open_status >= 80 and valve_open_status < 100:
                    self.size[f.fires_location_id] = self.size[f.fires_location_id] - 0.005
                
                if self.size[f.fires_location_id] > 0.0 and self.size[f.fires_location_id] <= 0.015:
                    self.level[f.fires_location_id] = 1
                elif self.size[f.fires_location_id] > 0.015 and self.size[f.fires_location_id] <= 0.023:
                    self.level[f.fires_location_id] = 2
                elif self.size[f.fires_location_id] > 0.023 and self.size[f.fires_location_id] <= 0.03:
                    self.level[f.fires_location_id] = 3
                elif self.size[f.fires_location_id] > 0.03 and self.size[f.fires_location_id] <= 0.04:
                    self.level[f.fires_location_id] = 4
                elif self.size[f.fires_location_id] > 0.04:
                    self.level[f.fires_location_id] = 5
                else:
                    self.size[f.fires_location_id] = 0.0
                    self.level[f.fires_location_id] = 0
            img = mpimg.imread(file_path)  # image file
            imagebox = OffsetImage(img, zoom=self.size[f.fires_location_id])
            ab = AnnotationBbox(imagebox, (f.fires_axis_x, f.fires_axis_y), frameon=False)
            ax.add_artist(ab)
        ax.set_xlim(-0.5, 11 - 0.5)
        ax.set_ylim(-0.5, 11 - 0.5)
        ax.set_xticks(range(11))
        ax.set_yticks(range(11))
        for i in range(map.shape[1]):
            for j in range(map.shape[0]):
                if map[i][j] == 1:
                    ax.fill_between([j, j+1], i, i+1, color='black', alpha=0.5)
                else:
                    ax.fill_between([j, j+1], i, i+1, color='white', alpha=0.5)
        ax.set_title("Reality Map 10x10")
        ax.plot(x, y, 'ro', markersize=15)
        #Tính toán vè vẽ người dùng
        #Chuyển hướng nhìn từ radian -> độ, Hướng gốc 0° của Wedge là trục X dương
        heading_deg = 90 - math.degrees(heading_angle_radians)
        theta1 = heading_deg - half_fov_deg
        theta2 = heading_deg + half_fov_deg
        wedge = patches.Wedge(center=(x, y), r=view_distance, theta1=theta1, theta2=theta2, facecolor='blue', alpha=0.5)
        ax.add_patch(wedge)
        buf = io.BytesIO()
        plt.savefig(buf, format='png')
        plt.close(fig)
        buf.seek(0)
        img_b64 = base64.b64encode(buf.read()).decode('utf-8')
        buf.close()
        return img_b64

