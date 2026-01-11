import numpy as np
import pandas as pd
import tensorflow as tf
from tensorflow.keras.models import load_model
from tensorflow.keras.layers import Dense, Dropout, Conv2D, Flatten, BatchNormalization
from tensorflow import keras
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.optimizers.schedules import ExponentialDecay
from sklearn.preprocessing import OneHotEncoder
from .wbo_filter import preprocessor, WBOFilter
import os
from sklearn.model_selection import train_test_split
from tensorflow.keras.callbacks import EarlyStopping

@keras.utils.register_keras_serializable()
class feedforward(tf.keras.Model):
    def __init__(self, num_classes=34, trainable=True, dtype="float32", **kwargs):
        super(feedforward, self).__init__(trainable=trainable, dtype=dtype, **kwargs)
        self.conv2d1 = Conv2D(16, (2,2), strides=(1, 1), activation='relu')
        self.batch_norm1 = BatchNormalization()
        self.conv2d2 = Conv2D(32, (2,2), strides=(1, 1), activation='relu')
        self.conv2d3 = Conv2D(32, (1,1), strides=(1, 1), activation='relu')
        self.conv2d4 = Conv2D(64, (1,1), strides=(1, 1), activation='relu')
        self.flatten = Flatten()
        self.dense1 = Dense(256, activation='relu')
        self.dropout = Dropout(0.3)
        self.dense3 = Dense(128, activation='relu') 
        self.dense4 = Dense(num_classes, activation='softmax')

    def call(self, inputs, training=False):
        x = self.conv2d1(inputs)
        x = self.batch_norm1(x, training=training)
        x = self.conv2d2(x)
        x = self.conv2d3(x)
        x = self.conv2d4(x)
        x = self.flatten(x)
        x = self.dense1(x)
        x = self.dropout(x, training=training)
        x = self.dense3(x)
        x = self.dense4(x)
        return x

class MLModel:
    def __init__(self, training_data, number_of_locations):
        self.model = feedforward(num_classes=number_of_locations)
        self.training_data = training_data
        self.number_of_locations = number_of_locations
        self.min = None
        self.max = None

    def load_model(self):
        BASE_DIR = os.path.dirname(os.path.abspath(__file__))
        file_path = os.path.join(BASE_DIR, 'model/ml_model.keras')  
        self.model = load_model(file_path, custom_objects={'feedforward': lambda **kwargs: feedforward(num_classes=self.number_of_locations, **kwargs)})
        BASE_DIR = os.path.dirname(os.path.abspath(__file__))
        file_path = os.path.join(BASE_DIR, 'training_data/data_preprocess.csv')
        data = pd.read_csv(file_path)
        data = data.to_numpy()
        X = data[:, :-1]
        X = X.reshape(-1, 6, 10)
        self.min = np.min(X, axis=(0, 2)).reshape(1, 6, 1)
        self.max = np.max(X, axis=(0, 2)).reshape(1, 6, 1)


    def preprocess_data(self):
        data_columns = ['rssi1', 'rssi2', 'rssi3', 'rssi4', 'magy', 'magz']  # Define rssi_columns
        unique_locations = self.training_data['location'].unique()
        all_x = []

        for loc in unique_locations:
            location_rows = self.training_data[self.training_data['location'] == loc]
            data = [np.array(location_rows[col]) for col in data_columns]
            X_loc = np.column_stack(data)
            rssi = np.column_stack(data)
            x_loc = []

            for i in range(0, (len(X_loc) - 9), 5):  # Ensure the loop does not exceed bounds
                # min_value = np.min(rssi[i:(i+10)], axis=0)
                # max_value = np.max(rssi[i:(i+10)], axis=0)
                # wbo_filters = [WBOFilter(int(min_value[l]), int(max_value[l])) for l in range(4)]
                # for j in range(10):
                #     for k in range(4):
                #         X_loc[i + j][k] = wbo_filters[k].proposed_filter(int(round(rssi[i + j][k])))
                x_loc.append(np.column_stack([X_loc[i + j] for j in range(10)]))

            x_loc = np.array(x_loc)
            all_x.append(x_loc)

        all_x = np.concatenate(all_x, axis=0)
        # Reshape all_x to (n, 40)
        reshaped_all_x = all_x.reshape(all_x.shape[0], -1)

        # Repeat the location column to match the number of rows in reshaped_all_x
        location_column = np.repeat(unique_locations, [(((len(self.training_data[self.training_data['location'] == loc]) - 10)/5)+1) for loc in unique_locations])

        # Ensure the length of location_column matches reshaped_all_x
        location_column = location_column[:reshaped_all_x.shape[0]].reshape(-1, 1)

        # Concatenate reshaped_all_x with the location column
        final_result = np.hstack((reshaped_all_x, location_column))
        final_dataframe = pd.DataFrame(final_result)
        BASE_DIR = os.path.dirname(os.path.abspath(__file__))
        file_path = os.path.join(BASE_DIR, 'training_data/data_preprocess.csv')
        final_dataframe.to_csv(file_path, index=False)

    def train_model(self):
        self.preprocess_data()
        BASE_DIR = os.path.dirname(os.path.abspath(__file__))
        file_path = os.path.join(BASE_DIR, 'training_data/data_preprocess.csv')
        data = pd.read_csv(file_path)
        data = data.to_numpy()
        X = data[:, :-1]
        y = data[:, -1]
        X = X.reshape(-1, 6, 10)
        print(X[0])
        min_vals = np.min(X, axis=(0, 2)).reshape(1, 6, 1)
        max_vals = np.max(X, axis=(0, 2)).reshape(1, 6, 1)
        X = (X - min_vals) / (max_vals - min_vals)
        X = np.expand_dims(X, axis=-1)
        encoder = OneHotEncoder(sparse_output=False)
        y = encoder.fit_transform(y.reshape(-1, 1))
        X_train, X_val, y_train, y_val = train_test_split(X, y, test_size=0.2, random_state=42)
        X_train = tf.convert_to_tensor(X_train, dtype=tf.float32)
        y_train = tf.convert_to_tensor(y_train, dtype=tf.float32)
        X_val = tf.convert_to_tensor(X_val, dtype=tf.float32)
        y_val = tf.convert_to_tensor(y_val, dtype=tf.float32)
        initial_learning_rate = 0.001
        lr_schedule = ExponentialDecay(
            initial_learning_rate = initial_learning_rate,
            decay_steps = 8000,
            decay_rate = 0.96,
        )
        early_stopping = EarlyStopping(
            monitor='val_loss',
            patience=15,
            restore_best_weights=True
        )
        opt = Adam(learning_rate = lr_schedule, beta_1 = 0.9, beta_2 = 0.999)
        self.model.compile(optimizer=opt, loss='categorical_crossentropy', metrics=['accuracy'])
        history = self.model.fit(X_train, y_train, epochs=100, batch_size=32, validation_data=(X_val, y_val))

        return history.history['accuracy'][-1], history.history['val_accuracy'][-1]
    
    def save_model(self):
        BASE_DIR = os.path.dirname(os.path.abspath(__file__))
        file_path = os.path.join(BASE_DIR, 'model/ml_model.keras')  
        self.model.save(file_path)




















# @keras.utils.register_keras_serializable()
# class feedforward(tf.keras.Model):
#     def __init__(self, num_classes=16, trainable=True, dtype="float32", **kwargs):
#         super(feedforward, self).__init__(trainable=trainable, dtype=dtype, **kwargs)
#         self.dense1 = Dense(512, activation='relu')
#         self.dropout1 = Dropout(0.3)
#         self.dense2 = Dense(512, activation='relu')
#         self.dropout2 = Dropout(0.3)
#         self.dense3 = Dense(256, activation='relu')
#         self.dropout3 = Dropout(0.2)
#         self.dense4 = Dense(128, activation='relu')
#         self.dropout4 = Dropout(0.1)
#         self.dense5 = Dense(128, activation='relu')
#         self.dense6 = Dense(num_classes, activation='softmax')

#     def call(self, inputs, training=False):
#         x = self.dense1(inputs)
#         x = self.dropout1(x, training=training)
#         x = self.dense2(x)
#         x = self.dropout2(x, training=training)
#         x = self.dense3(x)
#         x = self.dropout3(x, training=training)
#         x = self.dense4(x)
#         x = self.dense5(x)
#         return self.dense6(x)

# class MLModel:
#     def __init__(self, training_data, number_of_locations):
#         self.model = feedforward(num_classes=number_of_locations)
#         self.training_data = training_data
#         self.number_of_locations = number_of_locations

#     def load_model(self):
#         BASE_DIR = os.path.dirname(os.path.abspath(__file__))
#         file_path = os.path.join(BASE_DIR, 'model/ml_model.keras')  
#         self.model = load_model(file_path, custom_objects={'feedforward': lambda **kwargs: feedforward(num_classes=self.number_of_locations, **kwargs)})

#     def smoothing_data(self):
#         training_data = self.training_data
#         preprocess_data = preprocessor(training_data, self.number_of_locations)
#         preprocess_data.preprocess()
#         BASE_DIR = os.path.dirname(os.path.abspath(__file__))
#         file_path = os.path.join(BASE_DIR, 'training_data/WBOrssi.csv')
#         df = pd.read_csv(file_path)
#         data = df.sample(frac=1).reset_index(drop=True)
#         return data
    
#     def standardize_data(self):
#         # data = self.smoothing_data()
#         data = self.training_data
#         data = data.sample(frac=1).reset_index(drop=True)
#         input_colums = ['rssi1', 'rssi2', 'rssi3', 'rssi4']
#         output_colums = ['location']
#         X = data[input_colums]
#         y = data[output_colums]
#         X = np.array(X)
#         y = np.array(y)
#         X = (X - np.min(X, axis=0)) / (np.max(X, axis=0) - np.min(X, axis=0))
#         return X, y, np.min(X, axis=0), np.max(X, axis=0)

#     def train_model(self):
#         X, y, min, max = self.standardize_data()
#         encoder = OneHotEncoder(sparse_output=False)
#         y = encoder.fit_transform(y.reshape(-1, 1))
#         X = tf.convert_to_tensor(X, dtype=tf.float32)
#         y = tf.convert_to_tensor(y, dtype=tf.float32)
#         initial_learning_rate = 0.001
#         lr_schedule = ExponentialDecay(
#             initial_learning_rate = initial_learning_rate,
#             decay_steps = 8000,
#             decay_rate = 0.96,
#         )
#         opt = Adam(learning_rate = lr_schedule, beta_1 = 0.9, beta_2 = 0.999)
#         self.model.compile(optimizer=opt, loss='categorical_crossentropy', metrics=['accuracy'])
#         history = self.model.fit(X, y, epochs=200, batch_size=32, validation_split=0.2)

#         return history.history['accuracy'][-1], history.history['val_accuracy'][-1]
    
#     def save_model(self):
#         BASE_DIR = os.path.dirname(os.path.abspath(__file__))
#         file_path = os.path.join(BASE_DIR, 'model/ml_model.keras')  
#         self.model.save(file_path)