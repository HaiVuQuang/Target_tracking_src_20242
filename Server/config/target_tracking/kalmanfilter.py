import numpy as np
from collections import Counter

class step_detection:
    def __init__(self, upper_threshold, lower_threshold, min_time, max_time):
        self.upper_threshold = upper_threshold
        self.lower_threshold = lower_threshold
        self.min_time = min_time
        self.max_time = max_time
        self.high_peak_time = None
    def detect_step(self, value, time):
        if value >=5 and value <= -5:
            self.high_peak_time = None
        else:
            if value > self.upper_threshold and self.high_peak_time is None:
                self.high_peak_time = time
                return False
            elif value < self.lower_threshold and self.high_peak_time is not None:
                time_diff = time - self.high_peak_time
                if self.min_time <= time_diff <= self.max_time:
                    self.high_peak_time = None
                    return True
                elif time_diff > self.max_time:
                    self.high_peak_time = None
                    return False
                elif time_diff < self.min_time:
                    self.high_peak_time = None
                    return False
        return False

class linear_kalman_filter:
    def __init__(self, F, H, Q, R):
        self.F = F
        self.H = H
        self.Q = Q
        self.R = R
        self.state = None
        self.covariance = None
    
    def initialize(self, initial_state, initial_covariance):
        self.state = initial_state
        self.covariance = initial_covariance
    
    def update(self, measurement, control_input):
        self.state = self.F @ self.state + control_input
        self.covariance = self.F @ self.covariance @ self.F.T + self.Q
        kalman_gain = self.covariance @ self.H.T @ np.linalg.inv(self.H @ self.covariance @ self.H.T + self.R)
        self.state = self.state + kalman_gain @ (measurement - self.H @ self.state)
        self.covariance = (np.eye(len(self.state)) - kalman_gain @ self.H) @ self.covariance
        return self.state, self.covariance

class restricted_area:
    def __init__(self):
        self.x_center = 0
        self.y_center = 0
        self.x = None
        self.y = None
        self.wrong = []
        self.right_count = 0
        self.wrong_count = 0

    def reset_center(self, x, y):
        self.x_center = x
        self.y_center = y
        print("update")

    def set_positon(self, x, y):
        if x < (self.x_center + 1.5) and x > (self.x_center - 1.5) and y < (self.y_center + 1.5) and y > (self.y_center - 1.5):
            self.x = x
            self.y = y
            self.right_count += 1
            print("right:")
            print(self.right_count)
        else:
            self.x = None
            self.y = None
            a = np.array([x,y])
            self.wrong.append(a)
            self.wrong_count += 1
            print("wrong:")
            print(self.wrong_count)

        if self.right_count >= 5:
            self.wrong = []
            self.wrong_count = 0
            self.right_count = 0
            print("update right")
        if self.wrong_count >= 15:
            tuple_list = [tuple(arr.tolist()) for arr in self.wrong]
            counter = Counter(tuple_list)
            most_common_tuple, count = counter.most_common(1)[0]
            most_common_array = np.array(most_common_tuple)
            if count >= 5:
                self.x = most_common_array[0]
                self.y = most_common_array[1]
                self.x_center = self.x
                self.y_center = self.y
                self.wrong = []
                self.wrong_count = 0
                self.right_count = 0
                print("update wrong")
        return self.x, self.y