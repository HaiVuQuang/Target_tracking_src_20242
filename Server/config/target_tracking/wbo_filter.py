import numpy as np
import pandas as pd
import os
class WBOFilter:
    def __init__(self, min_value, max_value, a_factor=10, b_factor=3, threshold=0):
        if min_value == max_value:
            self.min_value = min_value - 1
            self.max_value = max_value + 1
            self.weights = np.ones(max_value - min_value + 3) * 1/(max_value - min_value + 3)
        else:
            self.min_value = min_value
            self.max_value = max_value
            self.weights = np.ones(max_value - min_value + 1) * 1/(max_value - min_value + 1)
        self.a_factor = a_factor
        self.b_factor = b_factor
        self.threshold = threshold
        
    def get_correlation_point(self):
        lower_margin = None
        upper_margin = None
        for i in range(len(self.weights)):
            if lower_margin is None and self.weights[i] > self.threshold:
                lower_margin = i
            if upper_margin is None and self.weights[-i] > self.threshold:
                upper_margin = len(self.weights) - (i + 1)
        return (upper_margin - lower_margin) / 2 + lower_margin
    
    def get_filter_value(self):
        min_weight = np.min(self.weights)
        adjusted_weights = self.weights - min_weight
        sum_of_weights = np.sum(adjusted_weights)
        values = np.arange(self.min_value, self.max_value + 1)
        sum_of_weighted_values = np.dot(adjusted_weights, values)
        return (sum_of_weighted_values / sum_of_weights)
    
    def proposed_filter(self, rssi):
        x = rssi - self.min_value
        correlation_point = self.get_correlation_point()

        new_weight = np.tanh(self.weights[x] + self.a_factor / (abs(x - correlation_point) + 1e-6))
        previous_weight = self.weights[x]
        self.weights[x] = new_weight

        for i in range(len(self.weights)):
            if i != x:
                self.weights[i] = np.tanh(self.weights[i] - self.b_factor * 
                abs(new_weight - previous_weight) / (self.max_value - self.min_value + 1))
        return self.get_filter_value()
    
class preprocessor:
    def __init__(self, data, number_of_locations):
        self.data = data
        self.number_of_locations = number_of_locations
    
    def preprocess(self):
        rssi_data = self.data
        rssi_1_data = {}
        rssi_2_data = {}
        rssi_3_data = {}
        rssi_4_data = {}
        for i in range(1, self.number_of_locations + 1):
            rssi_1 = rssi_data[rssi_data['location'] == i]['rssi1']
            min_rssi, max_rssi = rssi_1.min(), rssi_1.max()
            wbo_filter = WBOFilter(min_rssi, max_rssi)
            rssi_1_data[f'{i}'] = []
            for rssi in rssi_1:
                filtered_rssi = wbo_filter.proposed_filter(int(round(rssi)))
                rssi_1_data[f'{i}'].append(filtered_rssi)

            rssi_2 = rssi_data[rssi_data['location'] == i]['rssi2']
            min_rssi, max_rssi = rssi_2.min(), rssi_2.max()
            wbo_filter = WBOFilter(min_rssi, max_rssi)
            rssi_2_data[f'{i}'] = []
            for rssi in rssi_2:
                filtered_rssi = wbo_filter.proposed_filter(int(round(rssi)))
                rssi_2_data[f'{i}'].append(filtered_rssi)

            rssi_3 = rssi_data[rssi_data['location'] == i]['rssi3']
            min_rssi, max_rssi = rssi_3.min(), rssi_3.max()
            wbo_filter = WBOFilter(min_rssi, max_rssi)
            rssi_3_data[f'{i}'] = []
            for rssi in rssi_3:
                filtered_rssi = wbo_filter.proposed_filter(int(round(rssi)))
                rssi_3_data[f'{i}'].append(filtered_rssi)
                
            rssi_4 = rssi_data[rssi_data['location'] == i]['rssi4']
            min_rssi, max_rssi = rssi_4.min(), rssi_4.max()
            wbo_filter = WBOFilter(min_rssi, max_rssi)
            rssi_4_data[f'{i}'] = []
            for rssi in rssi_4:
                filtered_rssi = wbo_filter.proposed_filter(int(round(rssi)))
                rssi_4_data[f'{i}'].append(filtered_rssi)
        rssi1 = []
        rssi2 = []
        rssi3 = []
        rssi4 = []
        location = []
        for i in range(1, self.number_of_locations + 1):
            for j in range(len(rssi_1_data[f'{i}'])):
                rssi1.append(rssi_1_data[f'{i}'][j])
                rssi2.append(rssi_2_data[f'{i}'][j])
                rssi3.append(rssi_3_data[f'{i}'][j])
                rssi4.append(rssi_4_data[f'{i}'][j])
                location.append(i)
        df = pd.DataFrame({'rssi1': rssi1, 'rssi2': rssi2, 'rssi3': rssi3, 'rssi4': rssi4, 'location': location})
        BASE_DIR = os.path.dirname(os.path.abspath(__file__))
        file_path = os.path.join(BASE_DIR, 'rssi_data/WBOrssi.csv')  
        df.to_csv(file_path, index=False)

